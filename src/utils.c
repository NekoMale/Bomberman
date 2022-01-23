#include <SDL.h>

int Utils_OpenFile(const char* fileName, Uint8** content)
{
	SDL_RWops* rw = SDL_RWFromFile(fileName, "rb");
	if (!rw)
		return -1;

	Sint64 file_size = SDL_RWsize(rw);
	*content = SDL_malloc(file_size);
	if (!(*content))
	{
		SDL_RWclose(rw);
		return -1;
	}
	
	SDL_RWread(rw, *content, file_size, 1);
	SDL_RWclose(rw);
	return 0;
}