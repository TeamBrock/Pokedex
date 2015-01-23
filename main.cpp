#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "imgui.h"
#include "sqlite/sqlite3.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF) == -1) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		return 1;
	}

	if (TTF_Init() == -1) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Pokedex", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Surface *dexSurface = IMG_Load("assets/pokedex.png");
	if (dexSurface == nullptr) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
		return 1;
	}

	SDL_Texture *dexTexture = SDL_CreateTextureFromSurface(renderer, dexSurface);
	SDL_FreeSurface(dexSurface);
	if (dexTexture == nullptr) {
		SDL_DestroyTexture(dexTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	TTF_Font *font;
	font = TTF_OpenFont("assets/DroidSansMono.ttf", 12);
	if (font == nullptr) {
		std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		return 1;
	}

	//const SDL_Color white = {255, 255, 255};
	const SDL_Color black = {0, 0, 0};

	SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Hello World!", black);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);

	int textWidth, textHeight;
	SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
	textWidth *= 2; textHeight *=2;
	int initialHeight = WINDOW_HEIGHT/2 - textHeight;
	SDL_Rect textDest = {WINDOW_WIDTH/2 - textWidth/2, initialHeight, textWidth, textHeight};

	SDL_Event sdlEvent;
	bool sdlQuit = false;

	unsigned long timeElapsed = SDL_GetTicks();

	ui::Context con;

	while (!sdlQuit) {
		timeElapsed = SDL_GetTicks();

		// Poll for events
		while(SDL_PollEvent(&sdlEvent)) {
			switch(sdlEvent.type) {
				case SDL_QUIT:
					sdlQuit = true;
					break;
				case SDL_MOUSEMOTION:
					con.mouseX = sdlEvent.motion.x;
					con.mouseY = sdlEvent.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
						con.mouseDown = true;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (sdlEvent.button.button == SDL_BUTTON_LEFT)
						con.mouseDown = false;
					break;
			}
		}

		textDest.y = initialHeight + 5.0f*cos((float)timeElapsed/100.0f);

		// Draw calls
		SDL_RenderClear(renderer);
		//SDL_RenderCopy(renderer, dexTexture, nullptr, nullptr);

		con.begin();
		if (con.button(1, renderer, textDest.x, textDest.y, textDest.w, textDest.h)) {
			std::cout << "pressed button" << std::endl;
		}
		con.end();

		SDL_RenderCopy(renderer, textTexture, nullptr, &textDest);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(dexTexture);
	SDL_DestroyTexture(textTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
