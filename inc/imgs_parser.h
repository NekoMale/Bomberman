#include <SDL.h>

int ng_parser_bmp_to_texture(char* imageContent, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture);
int ng_parser_png_to_texture(char* imageContent, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture);