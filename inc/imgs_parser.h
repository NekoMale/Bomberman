#pragma once
#include <SDL.h>

#define ng_parser_bmp_to_texture(image_content, channels, renderer, texture) __ng_parser_bmp_to_texture(image_content, channels, renderer, texture)
#define ng_parser_png_to_texture(image_content, channels, renderer, texture) __ng_parser_png_to_texture(image_content, channels, renderer, texture)

int __ng_parser_bmp_to_texture(char* imageContent, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture);
int __ng_parser_png_to_texture(char* imageContent, Uint8 channels, SDL_Renderer* renderer, SDL_Texture** texture);