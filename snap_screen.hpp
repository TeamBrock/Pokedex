#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <ctime>

#include "screen.hpp"
#include "imgui.hpp"
#include "sprite.hpp"
#include "PokemonData.hpp"

class SnapScreen : public Screen {
public:
    SnapScreen();
    ~SnapScreen();
    bool initialize(RenderContext *context, ScreenDispatcher *dispatcher) override;
    void handleEvent(const SDL_Event &sdlEvent) override;
    void frameStep(unsigned long tickMS) override;
    
    void updateSprite(Uint32 dt, bool reverse);
    Uint32 to_millis(int minutes, int seconds);
    std::string to_time(Uint32 millis);
    int getRandom(int low, int high);
    SDL_Texture* textToTexture(std::string, SDL_Color*);
    SDL_Texture* imageToTexture(std::string path);
    SDL_Texture* createTransparentTexture(std::string path, SDL_Color *color);
    Uint32 distanceSquared();
    void updateCamera(int mx, int my);
	bool overlap(SDL_Rect& a, SDL_Rect& b);
private:
    //enum Dir { North, East, South, West };
    //typedef Uint32 Number;
    imgui::UIState m_userInterface;
    
    TTF_Font *font;
	SDL_Rect screenRect;
    
    //Timer
    SDL_Texture *timerTexture = nullptr;
    std::string timerText;
    SDL_Rect timerRect;
    int timerWidth;
    int timerHeight;
    SDL_Color red = { 255, 0, 0, 0 };
    //Score
    SDL_Texture *scoreTexture = nullptr;
    std::string scoreText;
    SDL_Rect scoreRect;
    int scoreWidth;
    int scoreHeight;
    SDL_Color blue = { 0, 0, 255, 0 };
    
    //Background
    SDL_Texture *background;
    SDL_Color white = { 0xff, 0xff, 0xff, 0 };
    
    //Bush
    SDL_Texture *bush;
    SDL_Rect bushes[4];
    int bushWidth = 180;
    int bushHeight = 180;
    
    //Crosshair
    SDL_Texture *crosshair;
    SDL_Rect crossRect;
    int crossWidth;
    int crossHeight;
    
    //Sprite simpleSprite;
    SDL_Texture *sprite;
    SDL_Rect spriteRect;
    int spriteWidth = 100;
    int spriteHeight = 100;
    bool spriteVisible;
    
    //Player's score
    int score;
    const static int HIT_POINTS = 5;
    //time of last frame
    Uint32 lastTime;
    //total game duration
    Uint32 countdown = 45 * 1000;//milliseconds
    
    //sprite regen timer
    Uint32 generationTime;
    Uint32 pauseTime;
    
    //Mouse pointer
    struct point {
        int mx;
        int my;
    };
    point cursor;
    //the current time in-game
    Uint32 gametime = 0;//milliseconds
    
    int phase;
    Uint32 spriteInterval = 3 * 1000;//milliseconds
    Uint32 spriteTravelDist = 200;//pixels
    Uint32 spriteHoldTime = 2 * 1000;//milliseconds
    Uint32 speed = 145;//pixels per second
    struct vec {
        int vx;
        int vy;
    };
    //direction vecs
    vec dir[4];
    int currentDir;
    int currentBush;
    
	point start_point;
    bool mouseClicked;
    
};
