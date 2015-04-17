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

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

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
    m_gwenCanvas->SetDrawBackground(true);
    m_gwenCanvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));

    // Create our unittest control (which is a Window with controls in it)
    m_pokedexBase = new Pokedex(m_gwenCanvas);
    m_gwenInput.Initialize(m_gwenCanvas);

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
	SDL_RenderClear(m_context->renderer);

	m_gwenRenderer->BeginContext(NULL);
	m_gwenCanvas->RenderCanvas();

	SDL_RenderPresent(m_context->renderer);
}
