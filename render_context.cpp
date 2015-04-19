#include <iostream>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "render_context.hpp"
#include "options.hpp"
#include "texture.hpp"

bool RenderContext::initalizeSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if (IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF) == -1) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() == -1) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow(
			"Pokédex",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			options::WINDOW_WIDTH,
			options::WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN);

	if (window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(
			window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	return true;
}

RenderContext::~RenderContext()
{
	destroy();
}

void RenderContext::destroy()
{
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}
	for (auto &k : m_textureCache) {
		SDL_DestroyTexture(k.second);
	}
	SDL_Quit();
}

void RenderContext::render(const Sprite &sprite)
{
	SDL_Rect dest = sprite.rect();
	SDL_RenderCopyEx(renderer, sprite.texture(), nullptr, &dest,
			sprite.angle(), nullptr, SDL_FLIP_NONE);
}

SDL_Texture *RenderContext::loadTexture(const std::string &texturePath)
{
	SDL_Texture *texture = nullptr;

	if (m_textureCache.count(texturePath) > 0) {
		texture = m_textureCache[texturePath];
	} else {
		texture = loadTextureFromFile(renderer, texturePath);
		m_textureCache.insert(std::pair<std::string, SDL_Texture*>(texturePath, texture));
	}

	return texture;
}
