#pragma once

#include <vector>
#include <SDL.h>
#include "render_context.hpp"

class Screen;

class ScreenDispatcher {
public:
	static const int HOMESCREEN = 0;
	static const int POKEDEXSCREEN = 1;

	bool initialize();
	void setToPokedexScreen();
	void tick();

private:
	std::vector<std::unique_ptr<Screen>> m_screens;
	RenderContext m_context;
	int m_currentScreen = 0;
	bool m_sdlQuit = false;
};
