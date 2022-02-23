#include <zconf.h>
#include <zlib.h>
#include "utils.h"
#include "linked_list.h"
#include "imgs_parser.h"

int __ng_parser_bmp_to_texture(char* bmp_content, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture)
{
	Uint8 alignment = 4;

	Uint8* pixelsHead = bmp_content + (*(bmp_content + 10));
	
	Uint32 width, height;
	SDL_memcpy(&width, bmp_content + 18, 4);
	SDL_memcpy(&height, bmp_content + 22, 4);

	Uint32 rowSize = width * channels;
	Uint32 paddedRowSize = (rowSize / alignment) * alignment;
	if(rowSize % alignment != 0)
	{
		paddedRowSize += alignment;
	}

	*texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, width, height);
	if (!*texture)
		return -1;

	Uint8* pixels = SDL_malloc(width * height * channels);
	if (!pixels)
	{
		SDL_free(*texture);
		return -1;
	}

	Uint32 x = 0;
	for (Sint32 y = height - 1; y > -1; --y)
	{
		SDL_memcpy(pixels + x * rowSize, pixelsHead + y * paddedRowSize, rowSize);
		++x;
	}
	SDL_UpdateTexture(*texture, NULL, pixels, rowSize);
	SDL_free(pixels);
	return 0;
}

typedef struct s_chunk
{
	Uint32 chunk_length;
	char chunk_type[5];
	Uint8* chunk_data;
} t_chunk;
typedef t_chunk* chunk;

int cmp_chunks(const void* o1, const void* o2)
{
	chunk c1 = (chunk)o1;
	chunk c2 = (chunk)o2;
	return SDL_strcmp(c1->chunk_type, c2->chunk_type) == 0 && c1->chunk_length == c2->chunk_length;
}

Uint8 recon_a(Uint8* recon, Uint8 stride, Uint8 bytes_per_pixel, Uint8 r, Uint8 c)
{
	if (c >= bytes_per_pixel)
	{
		return recon[r * stride + c - bytes_per_pixel];
	}
	return 0;
}

Uint8 recon_b(Uint8* recon, Uint8 stride, Uint8 r, Uint8 c)
{
	if (r > 0)
	{
		return recon[(r - 1) * stride + c];
	}
	return 0;
}

Uint8 recon_c(Uint8* recon, Uint8 stride, Uint8 bytes_per_pixel, Uint8 r, Uint8 c)
{
	if (r > 0 && c >= bytes_per_pixel)
	{
		return recon[(r - 1) * stride + c - bytes_per_pixel];
	}
	return 0;
}

Uint8 paeth_predictor(Uint8 a, Uint8 b, Uint8 c)
{
	Uint32 p = a + b - c;
	Uint32 pa = abs(p - a);
	Uint32 pb = abs(p - b);
	Uint32 pc = abs(p - c);
	Uint8 pr = c;
	if (pa <= pb && pa <= pc)
	{
		pr = a;
	}
	else if (pb <= pc)
	{
		pr = b;
	}
	return pr;
}

int __ng_parser_png_to_texture(char* png_content, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture)
{
	char* png_signature = "\x89PNG\r\n\x1a\n";
	size_t signature_length = SDL_strlen(png_signature);

	ng_list chunks = ng_list_new(t_chunk, cmp_chunks);
	
	Uint32 idat_index = 0;
	char* png_file_signature = (char*)SDL_malloc(signature_length + 1);
	SDL_strlcpy(png_file_signature, png_content, signature_length + 1);
	
	if (SDL_strcmp(png_signature, png_file_signature) == 0)
	{
		//SDL_Log("PNG signature valid (%u)", signature_length);
		Uint32 offset = 8;
	
		int index = 0;
		int data_size = 0;
	
		while (1)
		{
			Uint32 chunk_length;
			SDL_memcpy(&chunk_length, png_content + offset, 4);
			chunk_length = ng_utils_big_to_little_endian(chunk_length);
			//SDL_Log("Chunk Length:\t(%d) %d", offset, chunk_length);
			offset += 4;
	
			char chunk_type[5];
			SDL_strlcpy(chunk_type, png_content + offset, 5);
			chunk_type[4] = '\0';
			//SDL_Log("Chunk Type:\t(%d) %s", offset, chunk_type);
			offset += 4;
	
			Uint8* chunk_datas = SDL_malloc(chunk_length);
			SDL_memcpy(chunk_datas, png_content + offset, chunk_length);
			offset += chunk_length;
	
			Uint32 crc = crc32(0, chunk_type, 4);
			Uint32 checksum = crc32(crc, chunk_datas, chunk_length);
			//SDL_Log("Crc: %d | Checksum: %d", crc, checksum);
	
			Uint32 chunk_crc;
			SDL_memcpy(&chunk_crc, png_content + offset, 4);
			chunk_crc = ng_utils_big_to_little_endian(chunk_crc);
			//SDL_Log("Chunk Crc:\t(%d) %d", offset, chunk_crc);
			offset += 4;
	
			//unsigned int crc2 = crc32(0, (unsigned char*)"Hello World", 11);
			//printf("crc32 is %u\n", crc2);
	
			if (chunk_crc != checksum)
			{
				SDL_Log("%d != %d", chunk_crc, checksum);
				return -1;
			}
	
			chunk new_chunk = SDL_malloc(sizeof(t_chunk));
			new_chunk->chunk_length = chunk_length;
			SDL_memcpy(new_chunk->chunk_type, chunk_type, 4);
			new_chunk->chunk_type[4] = '\0';
			new_chunk->chunk_data = SDL_malloc(chunk_length);
			SDL_memcpy(new_chunk->chunk_data, chunk_datas, chunk_length);

			ng_list_add(chunks, new_chunk);
	
			//data_size += chunk_length;
			//if (index > 0)
			//{
			//	chunk bigger_chunks = SDL_realloc(chunks, sizeof(chunk) * (index + 1));
			//	chunks = bigger_chunks;
			//}
			////chunks[index].chunk_length = chunk_length;
			//SDL_memcpy(chunks[index].chunk_type, chunk_type, 4);
			//chunks[index].chunk_type[4] = '\0';
			//chunks[index].chunk_data = SDL_malloc(chunk_length);
			//SDL_memcpy(chunks[index].chunk_data, chunk_datas, chunk_length);
			////SDL_Log("/*******************************/");
	
			if (strcmp(chunk_type, "IDAT") == 0)
			{
				idat_index = index;
			}
	
			++index;
			if (strcmp(chunk_type, "IEND") == 0)
			{
				break;
			}
			SDL_free(chunk_datas);
		}
	
		chunk ihdr_chunk = (chunk)ng_list_get_at(chunks, 0);
		Uint32 width, height;
		SDL_memcpy(&width, ihdr_chunk->chunk_data + 0, 4);
		width = ng_utils_big_to_little_endian(width);
		SDL_memcpy(&height, ihdr_chunk->chunk_data + 4, 4);
		height = ng_utils_big_to_little_endian(height);
		//SDL_Log("%s Chunk: Width %d, Height %d", chunks[0].chunk_type, width, height);
		
		chunk idat_chunk = (chunk)ng_list_get_at(chunks, idat_index);
		Uint8 bitd, colort, compm, filterm, interlacem;
		SDL_memcpy(&bitd, idat_chunk->chunk_data + 8, 1);
		SDL_memcpy(&colort, idat_chunk->chunk_data + 9, 1);
		SDL_memcpy(&compm, idat_chunk->chunk_data + 10, 1);
		SDL_memcpy(&filterm, idat_chunk->chunk_data + 11, 1);
		SDL_memcpy(&interlacem, idat_chunk->chunk_data + 12, 1);
		//SDL_Log("%s Chunk: bitd %d, colort %d, compm %d, filterm %d, interlacem %d", chunks[0].chunk_type, bitd, colort, compm, filterm, interlacem);
		
		unsigned char* IDAT_data = SDL_malloc(idat_chunk->chunk_length + 1);
		SDL_memcpy(IDAT_data, idat_chunk->chunk_data, idat_chunk->chunk_length);
		IDAT_data[idat_chunk->chunk_length] = '\0';
		
		Uint32 uncompressed_size = height * (width * 4 + 1);
		Uint8* uncompressed_data = malloc(uncompressed_size);
		int result = uncompress(uncompressed_data, &uncompressed_size, idat_chunk->chunk_data, idat_chunk->chunk_length);
		
		if (result != Z_OK)
		{
			SDL_Log("unable to uncompress: error %d\n", result);
			return -1;
		}
		
		Uint8* recon = SDL_malloc(uncompressed_size);
		Uint32 bytes_per_pixel = 4;
		Uint32 stride = width * bytes_per_pixel;
		
		int idat = 0;
		int pixel = 0;
		for (Uint32 i = 0; i < height; ++i)
		{
			Uint8 filter_type = uncompressed_data[idat]; // first byte of scanline is filter type
			idat += 1;
			for (Uint32 j = 0; j < stride; ++j) // for each byte in scanline
			{
				Uint8 Filt_x = uncompressed_data[idat];
				idat += 1;
				Uint8 Recon_x = Filt_x;
				if (filter_type == 1) // Sub
				{
					Recon_x += recon_a(recon, stride, bytes_per_pixel, i, j);
				}
				else if (filter_type == 2) // Up
				{
					Recon_x += recon_b(recon, stride, i, j);
				}
				else if (filter_type == 3) // Average
				{
					Recon_x += recon_a(recon, stride, bytes_per_pixel, i, j) + recon_b(recon, stride, i, j);
				}
				else if (filter_type == 4) // Paeth
				{
					Uint8 a = recon_a(recon, stride, bytes_per_pixel, i, j);
					Uint8 b = recon_b(recon, stride, i, j);
					Uint8 c = recon_c(recon, stride, bytes_per_pixel, i, j);
					Uint8 pr = paeth_predictor(a, b, c);
					Recon_x = Filt_x + pr;
				}
				else if (filter_type != 0) {
					SDL_Log("Unknown filter type %d at rows %d", filter_type, i);
					return -1;
				}
				recon[pixel] = Recon_x & 0xff;
				++pixel;
			}
		}
		
		*texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
		if (!*texture)
		{
			return -1;
		}
		SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
		SDL_UpdateTexture(*texture, NULL, recon, width * 4);
		SDL_free(png_file_signature);
		png_file_signature = NULL;
		//for (int i = 0; i < index; ++i)
		//{
		//	SDL_free(chunks[i].chunk_data);
		//}
		//SDL_free(chunks);
		SDL_free(IDAT_data);
		IDAT_data = NULL;
		SDL_free(uncompressed_data);
		uncompressed_data = NULL;
		SDL_free(recon);
		recon = NULL;
	}
	else
	{
		SDL_Log("PNG signature invalid");
		return -1;
	}
	//SDL_Log("PNG read end");
	return 0;
}