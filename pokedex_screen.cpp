#include <string>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "options.hpp"
#include "render_context.hpp"
#include "imgui_sdlbackend.hpp"
#include "pokedex_screen.hpp"
#include "text.hpp"

using options::WINDOW_WIDTH;
using options::WINDOW_HEIGHT;

bool PokedexScreen::initialize(RenderContext *context)
{
	if (!Screen::initialize(context)) {
		return false;
	}

	m_font = TTF_OpenFont("assets/unifont-7.0.06.ttf", 16);
	if (m_font == nullptr) {
		std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		return false;
	}

	m_pokeData.setPokemon(1);
	m_currentPokemonID = m_pokeData.getID();
	setPokedexData(m_currentPokemonID);

	// initialize the user interface
	m_userInterface.setRenderBackend(std::make_unique<imgui::SDLRenderBackend>(m_context->renderer));
	return true;
}

void PokedexScreen::setPokedexData(int id)
{
	if (m_textTexture) {
		SDL_DestroyTexture(m_textTexture);
	}

	m_pokeData.setPokemon(id);
	// Pokemon data
	{
		std::string txt = "Name: " + m_pokeData.getName()
			+ "\n" + "Base HP: " + std::to_string(m_pokeData.getBaseHP())
			+ "\n" + "Base Att: " + std::to_string(m_pokeData.getBaseAtt())
			+ "\n" + "Base Def: " + std::to_string(m_pokeData.getBaseDef())
			+ "\n" + "Base Sp Att: " + std::to_string(m_pokeData.getBaseSpAtt())
			+ "\n" + "Base Sp Def: " + std::to_string(m_pokeData.getBaseSpDef())
			+ "\n\n" + m_pokeData.getFlavorText();

		m_textTexture = getTextRenderTexture(m_context->renderer, m_font, txt);
		m_pokemonName.setImage(m_textTexture);
	}

	// Pokemon image
	{
		m_pokemonImage = m_context->loadTexture(m_pokeData.getSpriteLocation());
		m_pokemonImage.setScale(3.0f);
		m_pokemonImage.setPosition(0, 100);
	}
}

void PokedexScreen::handleEvent(const SDL_Event &sdlEvent)
{
	switch(sdlEvent.type) {
		case SDL_MOUSEMOTION:
			m_userInterface.mouseX = sdlEvent.motion.x;
			m_userInterface.mouseY = sdlEvent.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
				m_userInterface.mouseDown = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				m_userInterface.mouseDown = false;
			break;
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
				case SDLK_EQUALS:
				case SDLK_KP_PLUS:
					m_currentPokemonID = (m_currentPokemonID % 150) + 1;
					setPokedexData(m_currentPokemonID);
					break;
				case SDLK_MINUS:
				case SDLK_KP_MINUS:
					m_currentPokemonID--;
					if (m_currentPokemonID < 1) {
						m_currentPokemonID = 151;
					}
					setPokedexData(m_currentPokemonID);
					break;
			}
			break;
	}

	m_userInterface.handleEvent(sdlEvent);
}

PokedexScreen::~PokedexScreen()
{
	TTF_CloseFont(m_font);
}

void PokedexScreen::frameStep(unsigned long)
{
	SDL_RenderClear(m_context->renderer);
	m_context->render(m_pokemonName);
	m_context->render(m_pokemonImage);

	m_userInterface.begin();
	if (m_userInterface.textField(1, 20, 300, 200, 20, m_textField)) {
	}
	m_userInterface.end();

	SDL_RenderPresent(m_context->renderer);
}
