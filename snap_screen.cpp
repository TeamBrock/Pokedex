
#include "snap_screen.hpp"

#include <string>
#include <iostream>
#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "options.hpp"
#include "render_context.hpp"
#include "imgui_sdlbackend.hpp"
#include "text.hpp"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

SnapScreen::SnapScreen() {

}

SnapScreen::~SnapScreen() {

}

bool SnapScreen::initialize(RenderContext *context, ScreenDispatcher *dispatcher) {
		if (!Screen::initialize(context, dispatcher)) {
				return false;
		}
		// initialize the user interface
		m_userInterface.setRenderBackend(std::make_unique<imgui::SDLRenderBackend>(m_context->renderer));
		font = TTF_OpenFont("assets/unifont-7.0.06.ttf", 32);

		//mouse cursor - centered
		cursor.mx = WINDOW_WIDTH * 0.5;
		cursor.my = WINDOW_HEIGHT * 0.5;

		screenRect = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };


		mouseClicked = false;
		score = 0;
		//timerText = std::to_string(countdown);
		timerText = to_time(countdown);
		timerTexture = textToTexture(timerText, &red);
		timerWidth = 150;
		timerHeight = 150;
		timerRect = {WINDOW_WIDTH - timerWidth - 10, 5, timerWidth, timerHeight }; //x,y,w,h

		scoreText = "0";
		scoreTexture = textToTexture(scoreText, &blue);
		scoreWidth = 40;
		scoreHeight = 40;
		scoreRect = {20, 5, scoreWidth, scoreHeight};

		background = imageToTexture("assets/pokyBackground.jpg");

		//simpleSprite.setImage(context->loadTexture("assets/sprites/Spr_5b_025.png"));
		//simpleSprite.setPosition(0,0);
		//simpleSprite.setScale(spriteWidth, spriteHeight);
		//spriteRect = simpleSprite.rect();
		sprite = imageToTexture("assets/sprites/Spr_5b_025.png");
		spriteRect = { 0,0, spriteWidth, spriteHeight };

		spriteVisible = false;

		crosshair = createTransparentTexture("assets/crosshair.png", &white);
		crossWidth = 40;
		crossHeight = 40;
		crossRect = {(int) (WINDOW_WIDTH * 0.5f) - 20, (int) (WINDOW_HEIGHT * 0.5f) - 20, crossWidth, crossHeight };

		//bush = imageToTexture("assets/bush.png");
		bush = createTransparentTexture("assets/bush.jpg", &white);
		bushes[0] = {(int) (WINDOW_WIDTH * 0.1), (int) (WINDOW_HEIGHT * 0.33),  bushWidth, bushHeight };
		bushes[1] = {(int) (WINDOW_WIDTH * 0.25), (int) (WINDOW_HEIGHT * 0.74), bushWidth, bushHeight };
		bushes[2] = {(int) (WINDOW_WIDTH * 0.55), (int) (WINDOW_HEIGHT * 0.25), bushWidth, bushHeight };
		bushes[3] = {(int) (WINDOW_WIDTH * 0.65), (int) (WINDOW_HEIGHT * 0.70), bushWidth, bushHeight };


		dir[0] = {  1,  0 };
		dir[1] = { -1,  0 };
		dir[2] = {  0,  1 };
		dir[3] = {  0, -1 };

		return true;
}

SDL_Texture* SnapScreen::textToTexture(std::string text, SDL_Color *color) {
		SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), *color);
		SDL_Texture* message = SDL_CreateTextureFromSurface(m_context->renderer, surface);
		SDL_FreeSurface(surface);
		return message;
}

SDL_Texture* SnapScreen::imageToTexture(std::string path) {
		SDL_Surface *surface = IMG_Load(path.c_str());
		if (!surface) {
				std::cerr << "Image error: " << IMG_GetError() << std::endl;
				return nullptr;
		}
		SDL_Texture *tex = SDL_CreateTextureFromSurface(m_context->renderer, surface);
		SDL_FreeSurface(surface);
		return tex;
}
SDL_Texture* SnapScreen::createTransparentTexture(std::string path, SDL_Color *color){
		SDL_Surface *surface = IMG_Load(path.c_str());

		if(!surface){
				std::cerr << "Image error: " << IMG_GetError() << std::endl;
				return nullptr;
		}
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, color->r, color->b, color->g));

		SDL_Texture *tex = SDL_CreateTextureFromSurface(m_context->renderer, surface);
		SDL_FreeSurface(surface);
		return tex;
}

void SnapScreen::handleEvent(const SDL_Event &sdlEvent) {
		static int mx;
		static int my;
		switch (sdlEvent.type) {
				case SDL_MOUSEBUTTONDOWN:
						mouseClicked = true;
						return;
				case SDL_MOUSEBUTTONUP:
						mouseClicked = false;
						return;
				case SDL_MOUSEMOTION:
						SDL_GetMouseState(&mx, &my);
						updateCamera(mx, my);
						return;
				case SDL_KEYDOWN:
						switch(sdlEvent.key.keysym.sym){
								case SDLK_ESCAPE:
										//back to home screen
										return;

						}
						return;
				default:
						return;
		}
}
void SnapScreen::updateCamera(int mx, int my){
		if(mx < crossRect.w){
				crossRect.x = 0;
		} else if(mx > WINDOW_WIDTH - crossRect.w){
				crossRect.x = WINDOW_WIDTH - crossRect.w;
		} else {
				crossRect.x = mx - (int)(0.5f * crossRect.w);
		}

		if(my < crossRect.h){
				crossRect.y = 0;
		} else if(my > WINDOW_HEIGHT - crossRect.h){
				crossRect.y = WINDOW_HEIGHT - crossRect.h;
		} else {
				crossRect.y = my - (int)(0.5f * crossRect.h);
		}
}

void SnapScreen::frameStep(unsigned long now) {
		if(lastTime == 0){
				lastTime = now;//how to init on every entry??
		}
		Uint32 elapsed = now - lastTime;
		gametime += elapsed;
		if (gametime > countdown) {
				//game over
				SDL_RenderClear(m_context->renderer);
				//purple = SDL_MapRGB()
				scoreText = std::to_string(score);
				scoreTexture = textToTexture(scoreText, &blue);
				scoreRect.x = (WINDOW_WIDTH * 0.5f) - (scoreRect.w * 0.5f);
				scoreRect.y = (WINDOW_HEIGHT * 0.5f) - (scoreRect.h * 0.5f);
				SDL_RenderCopy(m_context->renderer, scoreTexture, nullptr, &scoreRect);
				SDL_RenderPresent(m_context->renderer);
				return;
		}
		lastTime = now;
		if (spriteVisible) {
				switch (phase) {
						case 0:

								//std::cout << "bef x: " << spriteRect.x << " y: " << spriteRect.y << std::endl;
								updateSprite(elapsed, false);
								//std::cout << "aft x: " << spriteRect.x << " y: " << spriteRect.y << std::endl;
								//std::cout << "dist squared: " << distanceSquared() <<  std::endl;
								//std::cout << "ref dist: " << (spriteTravelDist * spriteTravelDist) <<  std::endl;
								if(distanceSquared() > (spriteTravelDist * spriteTravelDist)){
										phase = 1;
										pauseTime = 0;

								}
								break;
						case 1:
								pauseTime += elapsed;
								if(pauseTime > spriteHoldTime){
										phase = 2;
										//std::cout << "phase1 clear" << std::endl;
										start_point.mx = spriteRect.x;
										start_point.my = spriteRect.y;
								}
								break;
						case 2:
								updateSprite(elapsed, true);
								if(distanceSquared() > (spriteTravelDist * spriteTravelDist)){
										spriteVisible = false;

								}
				}
		} else {
				generationTime += elapsed;
				if(generationTime > spriteInterval){
						generationTime = 0;
						phase = 0;
						spriteVisible = true;
						currentBush = getRandom(0, 3);
						currentDir = getRandom(0, 3);
						spriteRect.x = bushes[currentBush].x + ((int)(bushes[currentBush].w * 0.5f) - (int)(spriteRect.w * 0.5f));
						spriteRect.y = bushes[currentBush].y + ((int)(bushes[currentBush].h * 0.5f) - (int)(spriteRect.h * 0.5f));
						start_point.mx = spriteRect.x;
						start_point.my = spriteRect.y;
				}
		}

		//rendering::

		//clear target
		SDL_RenderClear(m_context->renderer);
		//draw background
		SDL_RenderCopy(m_context->renderer, background, nullptr, nullptr); //stretch

		if (spriteVisible == true) {
				//m_context->render(simpleSprite);
				SDL_RenderCopy(m_context->renderer, sprite, nullptr, &spriteRect);
		}

		//draw bushes
		SDL_RenderCopy(m_context->renderer, bush, nullptr, &(bushes[0]));
		SDL_RenderCopy(m_context->renderer, bush, nullptr, &(bushes[1]));
		SDL_RenderCopy(m_context->renderer, bush, nullptr, &(bushes[2]));
		SDL_RenderCopy(m_context->renderer, bush, nullptr, &(bushes[3]));

		//draw timer
		timerText = to_time(countdown - gametime);
		timerTexture = textToTexture(timerText, &red);
		SDL_RenderCopy(m_context->renderer, timerTexture, nullptr, &timerRect);

		//draw score
		scoreText = std::to_string(score);
		scoreTexture = textToTexture(scoreText, &blue);
		SDL_RenderCopy(m_context->renderer, scoreTexture, nullptr, &scoreRect);


		//draw crosshair
		SDL_RenderCopy(m_context->renderer, crosshair, nullptr, &crossRect);

		//detect mouse hits
		if(spriteVisible && mouseClicked){
				//std::cout << "vis and click" << std::endl;
				if(overlap(spriteRect, crossRect)){
						score += HIT_POINTS;
						mouseClicked = false;
				}
		}

		//copy portion of texture to render target
		//SDL_RenderCopy(m_context->renderer, timerTexture, nullptr, &timerRect);
		//m_userInterface.end();

		//update screen with any rendering performed since last update
		SDL_RenderPresent(m_context->renderer);
}
bool SnapScreen::overlap(SDL_Rect& a, SDL_Rect& b){
			return (a.x < b.x + b.w) && (b.x < a.x + a.w) && (a.y < b.y + b.h) && (b.y < a.y + a.h);
}

//update spriteRect
void SnapScreen::updateSprite(Uint32 dt, bool reverse) {
		float delta = (dt / 1000.0f) * speed;
		spriteRect.x += (delta * dir[currentDir].vx * (reverse ? -1 : 1));
		spriteRect.y += (delta * dir[currentDir].vy * (reverse ? -1 : 1));
}

Uint32 SnapScreen::to_millis(int minutes, int seconds) {
		return ((minutes * 60) + seconds) * 1000;
}

int SnapScreen::getRandom(int low, int high) {
		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<> dist(low, high);
		return dist(engine);
}

Uint32 SnapScreen::distanceSquared() {
		SDL_Rect& a = spriteRect;
		return ((a.x - start_point.mx) * (a.x - start_point.mx)) + ((a.y - start_point.my) * (a.y - start_point.my));
}

std::string SnapScreen::to_time(Uint32 millis) {
		Uint32 totalSeconds = millis / 1000;
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;
		return std::to_string(minutes) + " : " + std::to_string(seconds);
}
