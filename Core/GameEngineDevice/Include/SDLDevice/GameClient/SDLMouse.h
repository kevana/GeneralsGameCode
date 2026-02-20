/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
*/

// FILE: SDLMouse.h ////////////////////////////////////////////////////////////
// SDL2-based mouse implementation for non-Windows platforms
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameClient/Mouse.h"
#include <SDL.h>
#include <vector>

class SDLMouse : public Mouse
{
public:
	SDLMouse();
	virtual ~SDLMouse();

	virtual void init(void);
	virtual void update(void);
	virtual void initCursorResources(void);
	virtual void setCursor(MouseCursor cursor);
	virtual void capture(void);
	virtual void releaseCapture(void);

	// Called from SDLGameEngine event loop to queue mouse events
	void pushMouseMotion(const SDL_MouseMotionEvent& event);
	void pushMouseButton(const SDL_MouseButtonEvent& event);
	void pushMouseWheel(const SDL_MouseWheelEvent& event);

protected:
	virtual UnsignedByte getMouseEvent(MouseIO *result, Bool flush);

private:
	struct SDLMouseEvent {
		ICoord2D pos;
		ICoord2D delta;
		Int wheelPos;
		MouseButtonState leftState;
		MouseButtonState rightState;
		MouseButtonState middleState;
		UnsignedInt time;
	};

	std::vector<SDLMouseEvent> m_eventQueue;
	unsigned m_eventIndex;
};
