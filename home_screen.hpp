#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "screen.hpp"
#include "imgui.hpp"
#include "sprite.hpp"

class HomeScreen : Screen {
public:
	HomeScreen() :
		m_dexDance(false) {}

	bool initialize(RenderContext *context) override;
	void destroy() override;
	void handleEvent(const SDL_Event &sdlEvent) override;
	void frameStep(unsigned long tickMS) override;
	void render(unsigned long tickMS) override;

private:
	bool m_dexDance = false;
	int m_initialHeight = 0;

	SDL_Rect m_textDest = EMPTY_RECT;
	SDL_Rect m_cartridgeDest = EMPTY_RECT;

	TTF_Font *m_font = nullptr;

	SDL_Texture *m_textTexture = nullptr;
	SDL_Texture *m_cartridgeTexture = nullptr;

	Sprite m_pokedexSprite;
	Sprite m_cartridgeSprite;
	imgui::UIState m_userInterface;
};
