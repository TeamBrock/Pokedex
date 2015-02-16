#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "screen.hpp"
#include "imgui.hpp"
#include "sprite.hpp"
#include "PokemonData.hpp"

class PokedexScreen : public Screen {
public:
	~PokedexScreen();

	bool initialize(RenderContext *context) override;
	void handleEvent(const SDL_Event &sdlEvent) override;
	void frameStep(unsigned long tickMS) override;
	void setPokedexData(int id);

private:
	imgui::UIState m_userInterface;
	TTF_Font *m_font = nullptr;
	SDL_Texture *m_textTexture = nullptr;
	PokemonData m_pokeData;
	std::string m_textField;
	Sprite m_pokemonImage;
	Sprite m_pokemonName;

	int m_currentPokemonID;
};
