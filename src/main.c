#include <SDL.h>
#include <stdio.h>
#include "utils.h"
#include "string.h"
#include "imgs_parser.h"
#include "bomberman.h"
#include "level001.h"

int parser_png(Uint8* imageContent, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture)
{
	Uint8 alignment = 4;

	Uint8* pixelsHead = imageContent + (*(imageContent + 10));

	Uint32 width, height;
	SDL_memcpy(&width, imageContent + 18, 4);
	SDL_memcpy(&height, imageContent + 22, 4);

	Uint32 rowSize = width * channels;
	Uint32 paddedRowSize = (rowSize / alignment) * alignment;
	if (rowSize % alignment != 0)
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

int main(int argc, char** argv)
{

	level_t level001;
	level_init(&level001, 8, 8, 64, level001_cells);

	bomberman_t player0;
	player0.movable.x = 100;
	player0.movable.y = 100;
	player0.movable.width = 32;
	player0.movable.height = 32;
	player0.movable.speed = 48;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, level001.cols * level001.cell_size, level001.rows * level001.cell_size, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	
	SDL_Rect cell_rect = { 1, 1, level001.cell_size, level001.cell_size };
	SDL_Rect player0_rect = { 1, 1, player0.movable.width, player0.movable.height };
	SDL_Rect prova_rect = { 1, 1, 32, 32 };

	float delta_right = 0;
	float delta_left = 0;
	float delta_down = 0;
	float delta_up = 0;

	SDL_Texture* texture;
	char* content;
	if (ng_utils_open_file("assets/Peasant_Right.bmp", &content))
		goto quit;
	if (ng_parser_bmp_to_texture(content, 3, renderer, &texture))
		goto quit;


	SDL_Texture* texture2;
	char* png_content;
	if (ng_utils_open_file("assets/basn6a08.png", &png_content))
		goto quit;
	if (ng_parser_png_to_texture(png_content, 4, renderer, &texture2))
		goto quit;

	int running = 1;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = 0;
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					delta_right = player0.movable.speed * (1.0 / 60);
				}
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					delta_left = -player0.movable.speed * (1.0 / 60);
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					delta_down = player0.movable.speed * (1.0 / 60);
				}
				if (event.key.keysym.sym == SDLK_UP)
				{
					delta_up = -player0.movable.speed * (1.0 / 60);
				}
			}
			if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					delta_right = 0;
				}
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					delta_left = 0;
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					delta_down = 0;
				}
				if (event.key.keysym.sym == SDLK_UP)
				{
					delta_up = 0;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 1, 1, 1, 0);
		SDL_RenderClear(renderer);

		for (uint32_t row = 0; row < level001.rows; row++)
		{
			for (uint32_t col = 0; col < level001.cols; col++)
			{
				int32_t cell = level_cell(&level001, col, row);
				int32_t cell_texture = cell & 0xff;
				cell_rect.x = col * level001.cell_size;
				cell_rect.y = row * level001.cell_size;
				if (cell_texture == BLOCK_GROUND)
				{
					SDL_SetRenderDrawColor(renderer, 1, 255, 1, 255);
					SDL_RenderFillRect(renderer, &cell_rect);
				}
				else if (cell_texture == BLOCK_WALL)
				{
					SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);
					SDL_RenderFillRect(renderer, &cell_rect);
				}
				else if (cell_texture == BLOCK_DESTROYABLE)
				{
					SDL_SetRenderDrawColor(renderer, 0, 50, 50, 255);
					SDL_RenderFillRect(renderer, &cell_rect);
				}
			}
		}

		move_on_level(&level001, &player0.movable, delta_right + delta_left, delta_down + delta_up);
		player0_rect.x = player0.movable.x;
		player0_rect.y = player0.movable.y;
		SDL_RenderCopy(renderer, texture, NULL, &player0_rect);
		
		SDL_RenderCopy(renderer, texture2, NULL, &prova_rect);

		SDL_RenderPresent(renderer);
	}

	SDL_Log("End reached");
quit:
	//if (texture) SDL_DestroyTexture(texture);
	//if (renderer) SDL_DestroyRenderer(renderer);
	//if (window) SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}