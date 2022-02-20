#pragma once
#include <SDL.h>

int ng_utils_open_file(const char* fileName, char** content)
{
	SDL_RWops* rw = SDL_RWFromFile(fileName, "rb");
	if (!rw)
	{
		return -1;
	}

	Sint64 file_size = SDL_RWsize(rw);
	*content = (char*)SDL_malloc(file_size);
	if (!(*content))
	{
		SDL_RWclose(rw);
		return -1;
	}

	SDL_RWread(rw, *content, file_size, 1);
	SDL_RWclose(rw);
	return 0;
}

Uint32 ng_utils_big_to_little_endian(Uint32 n)
{
	return	((n >> 24) & 0x000000ff) | // move byte 3 to byte 0
			((n << 24) & 0xff000000) | // byte 0 to byte 3
			((n >> 8) & 0x0000ff00) | // move byte 2 to byte 1
			((n << 8) & 0x00ff0000); // move byte 1 to byte 2
}