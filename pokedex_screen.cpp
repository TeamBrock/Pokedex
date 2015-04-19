#include <string>
#include <iostream>
#include <regex>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "options.hpp"
#include "render_context.hpp"
#include "imgui_sdlbackend.hpp"
#include "pokedex_screen.hpp"
#include "text.hpp"
#include "pokedex.hpp"
#include "texture.hpp"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

// Draws a grid to the surface by drawing horizontal and vertical lines with each
// cell with a width and height of 'size'
static inline void createGridSurface(SDL_Surface *screen, Uint32 color, int size) {
	for(int y = 0; y < screen->h; y++) {
		for (int x = 0; x < screen->w; x++) {
			if (x % size == 0)
				putPixel(screen, x, y, color);
		}     
	}

	for(int x = 0; x < screen->w; x++) {
		for (int y = 0; y < screen->h; y++) {
			if (y % size == 0)
				putPixel(screen, x, y, color);
		}     
	}
}

static SDL_Rect gridRect;

bool PokedexScreen::initialize(RenderContext *context, ScreenDispatcher *dispatcher)
{
	if (!Screen::initialize(context, dispatcher)) {
		return false;
	}

    m_gwenRenderer = new Gwen::Renderer::SDL2(m_context->window, m_context->renderer);
	m_gwenSkin = new Gwen::Skin::TexturedBase(m_gwenRenderer);
    m_gwenSkin->SetRender(m_gwenRenderer);
    m_gwenSkin->Init("assets/DefaultSkin.png");
    m_gwenSkin->SetDefaultFont(pokedexFont, bigFont);
    
    m_gwenCanvas = new Gwen::Controls::Canvas(m_gwenSkin);
    m_gwenCanvas->SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_gwenCanvas->SetDrawBackground(false);
    //m_gwenCanvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));

    // Create our unittest control (which is a Window with controls in it)
    m_pokedexBase = new Pokedex(m_gwenCanvas);
    m_gwenInput.Initialize(m_gwenCanvas);


	// Create grid texture
	Uint32 gmask, rmask, bmask, amask;
	getMasks(&gmask, &rmask, &bmask, &amask);
	SDL_Surface *gridSurface = SDL_CreateRGBSurface(
		0, WINDOW_WIDTH, WINDOW_HEIGHT,
		32, rmask, gmask, bmask, amask
	);
	Uint32 color = SDL_MapRGBA(gridSurface->format, 75, 67, 142, 255);
	createGridSurface(gridSurface,  color, WINDOW_WIDTH/40);
	gridTexture = SDL_CreateTextureFromSurface(m_context->renderer, gridSurface);
	SDL_FreeSurface(gridSurface);

	gridRect.x = 0;
	gridRect.y = 0;
	gridRect.w = WINDOW_WIDTH;
	gridRect.h = WINDOW_HEIGHT;

	return true;
}

void PokedexScreen::handleEvent(const SDL_Event &sdlEvent)
{
	switch(sdlEvent.type) {
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			}
			break;
	}

	m_gwenInput.ProcessEvent(const_cast<SDL_Event*>(&sdlEvent));
}

PokedexScreen::~PokedexScreen()
{
	delete m_gwenCanvas;
	delete m_gwenSkin;
	delete m_gwenRenderer;
}

void PokedexScreen::frameStep(unsigned long)
{
	SDL_SetRenderDrawColor(m_context->renderer, 255, 255, 255, 255);
	SDL_RenderClear(m_context->renderer);
	SDL_RenderCopy(m_context->renderer, gridTexture, nullptr, &gridRect);

	m_gwenRenderer->BeginContext(NULL);
	m_gwenCanvas->RenderCanvas();

	SDL_RenderPresent(m_context->renderer);
}
