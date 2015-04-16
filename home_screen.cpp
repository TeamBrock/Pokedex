#include <string>
#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "render_context.hpp"
#include "options.hpp"
#include "home_screen.hpp"
#include "imgui_sdlbackend.hpp"
#include "text.hpp"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

bool HomeScreen::initialize(RenderContext *context, ScreenDispatcher *dispatcher)
{
	m_textDest.x = 0; m_textDest.y = 0; m_textDest.w = 0; m_textDest.h = 0;

	if (!Screen::initialize(context, dispatcher)) {
		return false;
	}

	// initialize the user interface
	m_userInterface.setRenderBackend(std::make_unique<imgui::SDLRenderBackend>(m_context->renderer));

	// Pokedex sprite
	{
		m_pokedexSprite.setImage(context->loadTexture("assets/pokedex.png"));
		m_pokedexSprite.setScale(5);
		SDL_Rect dexRect = m_pokedexSprite.rect();
		m_pokedexSprite.setPosition(WINDOW_WIDTH/2 - dexRect.w/2, WINDOW_HEIGHT/3 - dexRect.h/2);
	}

	// Pokemon Snap Cartridge sprite
	{
		m_cartridgePokemonSnap.setImage(context->loadTexture("assets/cartridge.png"));
		m_cartridgePokemonSnap.setScale(2);
		SDL_Rect cartridgeRect = m_cartridgePokemonSnap.rect();
		SDL_Rect dexRect = m_pokedexSprite.rect();
		m_cartridgePokemonSnap.setPosition((dexRect.w - (2*cartridgeRect.w+10))/2 + dexRect.x,
				dexRect.y + dexRect.h + 15);
	}

	// Quiz Cartridge sprite
	{
		m_cartridgeQuiz.setImage(context->loadTexture("assets/cartridge.png"));
		m_cartridgeQuiz.setScale(2);
		SDL_Rect cartridgeRect = m_cartridgeQuiz.rect();
		SDL_Rect dexRect = m_pokedexSprite.rect();
		m_cartridgeQuiz.setPosition(m_cartridgePokemonSnap.rect().x + cartridgeRect.w + 10,
				dexRect.y + dexRect.h + 15);
	}

	// Pokeball sprite
	{
		SDL_Texture *pokeballTexture = context->loadTexture("assets/pokeball_background.png");
		SDL_SetTextureAlphaMod(pokeballTexture, 90);
		m_pokeball.setImage(pokeballTexture);
		m_pokeball.setScale(0.5f);
	}

	// Font initialization
	// TODO: move to render context - stephen
	m_font = TTF_OpenFont("assets/unifont-7.0.06.ttf", 16);
	if (m_font == nullptr) {
		std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		return false;
	}

	SDL_SetRenderDrawColor(context->renderer, 75, 67, 142, 255);

	return true;
}

void HomeScreen::onEnter()
{
	SDL_SetRenderDrawColor(m_context->renderer, 75, 67, 142, 255);
}

void HomeScreen::handleEvent(const SDL_Event &sdlEvent)
{
	m_userInterface.handleEvent(sdlEvent);
}

HomeScreen::~HomeScreen()
{
}

void HomeScreen::frameStep(unsigned long elapsedMS)
{
	static unsigned long lastTime = 0;
	float deltaS = static_cast<float>(elapsedMS - lastTime)/1000.0f;
	lastTime = elapsedMS;

	m_textDest.y = m_initialHeight + 5.0f*cos((float)elapsedMS/100.0f);

	SDL_RenderClear(m_context->renderer);

	// Render background
	{
		SDL_Rect ballRect = m_pokeball.rect();

		m_pokeball.setAngle(10.0f*cos(static_cast<float>(elapsedMS/500.0f)));
		for (int y = -1; y < WINDOW_HEIGHT/ballRect.h + 1; y++) {
			for (int x = -1; x < WINDOW_WIDTH/ballRect.w + 1; x++) { 
				float newX = static_cast<float>(x*ballRect.w);
				float newY = static_cast<float>(y*ballRect.h);
				m_pokeball.setPosition(newX, newY);
				m_context->render(m_pokeball);
			}
		}
	}

	if (m_dexDance) {
		m_pokedexSprite.setAngle(5.0f*cos(static_cast<float>(elapsedMS/100.0f)));
	} else if (m_pokedexSprite.angle() != 0) {
		float angle = m_pokedexSprite.angle();
		m_pokedexSprite.setAngle(angle + (0 - angle) * 10.0f * deltaS);
	}

	// User interface controls
	m_userInterface.begin();
	if (m_userInterface.button(1, m_textDest.x, m_textDest.y, m_textDest.w, m_textDest.h)) {
		m_dexDance = !m_dexDance;
	}

	if (m_userInterface.mouseOverSprite(m_pokedexSprite)) {
		m_dexDance = true;
	} else {
		m_dexDance = false;
	}

	if (m_userInterface.mouseOverSprite(m_pokedexSprite) && m_userInterface.mouseDown) {
		m_pokedexSprite.setScale(4);
		SDL_Rect dexRect = m_pokedexSprite.rect();
		m_pokedexSprite.setPosition(WINDOW_WIDTH/2 - dexRect.w/2, WINDOW_HEIGHT/3 - dexRect.h/2);
	} else {
		m_pokedexSprite.setScale(5);
		SDL_Rect dexRect = m_pokedexSprite.rect();
		m_pokedexSprite.setPosition(WINDOW_WIDTH/2 - dexRect.w/2, WINDOW_HEIGHT/3 - dexRect.h/2);
	}

	if (m_userInterface.clickedSprite(2, m_pokedexSprite)) {
		m_screenDispatcher->setToPokedexScreen();	
	}

	// Render image screen elements
	m_context->render(m_cartridgePokemonSnap);
	m_context->render(m_cartridgeQuiz);
	m_context->render(m_pokedexSprite);

	SDL_RenderCopy(m_context->renderer, m_textTexture, nullptr, &m_textDest);
	m_userInterface.end();

	SDL_RenderPresent(m_context->renderer);
}
