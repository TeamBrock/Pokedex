#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "screen.hpp"

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

class Pokedex;
class PokedexScreen : public Screen {
public:
	~PokedexScreen();

	bool initialize(RenderContext *context, ScreenDispatcher *dispatcher) override;
	void handleEvent(const SDL_Event &sdlEvent) override;
	void frameStep(unsigned long tickMS) override;
	void setPokedexData(int id);

private:
	Gwen::Renderer::SDL2 *m_gwenRenderer = nullptr;
	Gwen::Controls::Canvas *m_gwenCanvas = nullptr;
	Gwen::Skin::TexturedBase *m_gwenSkin = nullptr;
	Pokedex *m_pokedexBase= nullptr;
	SDL_Texture *gridTexture;
    Gwen::Input::SDL2 m_gwenInput;
};
