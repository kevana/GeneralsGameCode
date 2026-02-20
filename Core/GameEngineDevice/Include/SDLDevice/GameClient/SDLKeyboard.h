/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
*/

// FILE: SDLKeyboard.h ////////////////////////////////////////////////////////
// SDL2-based keyboard implementation for non-Windows platforms
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameClient/Keyboard.h"
#include <SDL.h>
#include <vector>

class SDLKeyboard : public Keyboard
{
public:
	SDLKeyboard();
	virtual ~SDLKeyboard();

	virtual void init(void);
	virtual void update(void);
	virtual Bool getCapsState(void);

	// Called from SDLGameEngine event loop to queue key events
	void pushKeyEvent(const SDL_KeyboardEvent& event);

protected:
	virtual void getKey(KeyboardIO *key);

private:
	static UnsignedByte translateSDLScancode(SDL_Scancode sc);

	struct SDLKeyEvent {
		UnsignedByte key;      // DirectInput-compatible scan code
		UnsignedShort state;   // KEY_STATE_UP or KEY_STATE_DOWN
	};

	std::vector<SDLKeyEvent> m_eventQueue;
	unsigned m_eventIndex;
};
