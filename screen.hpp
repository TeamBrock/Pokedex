#pragma once

#include <SDL.h>
#include "screen_dispatcher.hpp"

class Screen {
// I prefer POD, but for this project, it may be too complex
public:
	Screen() {}
	Screen(const Screen&) = delete;
	Screen& operator=(const Screen&) = delete;

	virtual ~Screen() {}
	// defer initialization because of emplace_back noexcept
	virtual bool initialize(RenderContext *context, ScreenDispatcher *dispatcher)
	{
		m_context = context;
		m_screenDispatcher = dispatcher;
		return true;
	}
	virtual void handleEvent(const SDL_Event &) {}
	virtual void frameStep(unsigned long tickMS) = 0;
	virtual void onEnter() {};

protected:
	RenderContext *m_context;
	ScreenDispatcher *m_screenDispatcher;
};
