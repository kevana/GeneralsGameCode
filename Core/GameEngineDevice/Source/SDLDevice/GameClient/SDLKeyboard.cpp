/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
*/

// FILE: SDLKeyboard.cpp //////////////////////////////////////////////////////
// SDL2-based keyboard implementation for non-Windows platforms.
// Translates SDL scancodes to DirectInput scan codes used by the engine.
////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"
#include "SDLDevice/GameClient/SDLKeyboard.h"

SDLKeyboard::SDLKeyboard()
	: m_eventIndex(0)
{
}

SDLKeyboard::~SDLKeyboard()
{
}

void SDLKeyboard::init(void)
{
	Keyboard::init();
	m_eventQueue.clear();
	m_eventIndex = 0;
}

void SDLKeyboard::update(void)
{
	// Reset event index for this frame; the base class update will call getKey()
	m_eventIndex = 0;

	// Let the base class handle key state tracking
	Keyboard::update();

	// Clear consumed events
	m_eventQueue.clear();
	m_eventIndex = 0;
}

Bool SDLKeyboard::getCapsState(void)
{
	return (SDL_GetModState() & KMOD_CAPS) != 0;
}

void SDLKeyboard::pushKeyEvent(const SDL_KeyboardEvent& event)
{
	UnsignedByte dik = translateSDLScancode(event.keysym.scancode);
	if (dik == 0) return;

	SDLKeyEvent ke;
	ke.key = dik;
	ke.state = (event.type == SDL_KEYDOWN) ? KEY_STATE_DOWN : KEY_STATE_UP;
	m_eventQueue.push_back(ke);
}

void SDLKeyboard::getKey(KeyboardIO *key)
{
	if (m_eventIndex >= m_eventQueue.size()) {
		key->key = KEY_NONE;
		key->state = KEY_STATE_NONE;
		return;
	}

	const SDLKeyEvent& ev = m_eventQueue[m_eventIndex++];
	key->key = ev.key;
	key->state = ev.state;
	key->status = KeyboardIO::STATUS_USED;
}

// Translate SDL_Scancode to DirectInput DIK_* scan code
UnsignedByte SDLKeyboard::translateSDLScancode(SDL_Scancode sc)
{
	switch (sc) {
		case SDL_SCANCODE_ESCAPE:       return DIK_ESCAPE;
		case SDL_SCANCODE_1:            return DIK_1;
		case SDL_SCANCODE_2:            return DIK_2;
		case SDL_SCANCODE_3:            return DIK_3;
		case SDL_SCANCODE_4:            return DIK_4;
		case SDL_SCANCODE_5:            return DIK_5;
		case SDL_SCANCODE_6:            return DIK_6;
		case SDL_SCANCODE_7:            return DIK_7;
		case SDL_SCANCODE_8:            return DIK_8;
		case SDL_SCANCODE_9:            return DIK_9;
		case SDL_SCANCODE_0:            return DIK_0;
		case SDL_SCANCODE_MINUS:        return DIK_MINUS;
		case SDL_SCANCODE_EQUALS:       return DIK_EQUALS;
		case SDL_SCANCODE_BACKSPACE:    return DIK_BACK;
		case SDL_SCANCODE_TAB:          return DIK_TAB;
		case SDL_SCANCODE_Q:            return DIK_Q;
		case SDL_SCANCODE_W:            return DIK_W;
		case SDL_SCANCODE_E:            return DIK_E;
		case SDL_SCANCODE_R:            return DIK_R;
		case SDL_SCANCODE_T:            return DIK_T;
		case SDL_SCANCODE_Y:            return DIK_Y;
		case SDL_SCANCODE_U:            return DIK_U;
		case SDL_SCANCODE_I:            return DIK_I;
		case SDL_SCANCODE_O:            return DIK_O;
		case SDL_SCANCODE_P:            return DIK_P;
		case SDL_SCANCODE_LEFTBRACKET:  return DIK_LBRACKET;
		case SDL_SCANCODE_RIGHTBRACKET: return DIK_RBRACKET;
		case SDL_SCANCODE_RETURN:       return DIK_RETURN;
		case SDL_SCANCODE_LCTRL:        return DIK_LCONTROL;
		case SDL_SCANCODE_A:            return DIK_A;
		case SDL_SCANCODE_S:            return DIK_S;
		case SDL_SCANCODE_D:            return DIK_D;
		case SDL_SCANCODE_F:            return DIK_F;
		case SDL_SCANCODE_G:            return DIK_G;
		case SDL_SCANCODE_H:            return DIK_H;
		case SDL_SCANCODE_J:            return DIK_J;
		case SDL_SCANCODE_K:            return DIK_K;
		case SDL_SCANCODE_L:            return DIK_L;
		case SDL_SCANCODE_SEMICOLON:    return DIK_SEMICOLON;
		case SDL_SCANCODE_APOSTROPHE:   return DIK_APOSTROPHE;
		case SDL_SCANCODE_GRAVE:        return DIK_GRAVE;
		case SDL_SCANCODE_LSHIFT:       return DIK_LSHIFT;
		case SDL_SCANCODE_BACKSLASH:    return DIK_BACKSLASH;
		case SDL_SCANCODE_Z:            return DIK_Z;
		case SDL_SCANCODE_X:            return DIK_X;
		case SDL_SCANCODE_C:            return DIK_C;
		case SDL_SCANCODE_V:            return DIK_V;
		case SDL_SCANCODE_B:            return DIK_B;
		case SDL_SCANCODE_N:            return DIK_N;
		case SDL_SCANCODE_M:            return DIK_M;
		case SDL_SCANCODE_COMMA:        return DIK_COMMA;
		case SDL_SCANCODE_PERIOD:       return DIK_PERIOD;
		case SDL_SCANCODE_SLASH:        return DIK_SLASH;
		case SDL_SCANCODE_RSHIFT:       return DIK_RSHIFT;
		case SDL_SCANCODE_KP_MULTIPLY:  return DIK_NUMPADSTAR;
		case SDL_SCANCODE_LALT:         return DIK_LALT;
		case SDL_SCANCODE_SPACE:        return DIK_SPACE;
		case SDL_SCANCODE_CAPSLOCK:     return DIK_CAPSLOCK;
		case SDL_SCANCODE_F1:           return DIK_F1;
		case SDL_SCANCODE_F2:           return DIK_F2;
		case SDL_SCANCODE_F3:           return DIK_F3;
		case SDL_SCANCODE_F4:           return DIK_F4;
		case SDL_SCANCODE_F5:           return DIK_F5;
		case SDL_SCANCODE_F6:           return DIK_F6;
		case SDL_SCANCODE_F7:           return DIK_F7;
		case SDL_SCANCODE_F8:           return DIK_F8;
		case SDL_SCANCODE_F9:           return DIK_F9;
		case SDL_SCANCODE_F10:          return DIK_F10;
		case SDL_SCANCODE_NUMLOCKCLEAR: return DIK_NUMLOCK;
		case SDL_SCANCODE_SCROLLLOCK:   return DIK_SCROLL;
		case SDL_SCANCODE_KP_7:         return DIK_NUMPAD7;
		case SDL_SCANCODE_KP_8:         return DIK_NUMPAD8;
		case SDL_SCANCODE_KP_9:         return DIK_NUMPAD9;
		case SDL_SCANCODE_KP_MINUS:     return DIK_NUMPADMINUS;
		case SDL_SCANCODE_KP_4:         return DIK_NUMPAD4;
		case SDL_SCANCODE_KP_5:         return DIK_NUMPAD5;
		case SDL_SCANCODE_KP_6:         return DIK_NUMPAD6;
		case SDL_SCANCODE_KP_PLUS:      return DIK_NUMPADPLUS;
		case SDL_SCANCODE_KP_1:         return DIK_NUMPAD1;
		case SDL_SCANCODE_KP_2:         return DIK_NUMPAD2;
		case SDL_SCANCODE_KP_3:         return DIK_NUMPAD3;
		case SDL_SCANCODE_KP_0:         return DIK_NUMPAD0;
		case SDL_SCANCODE_KP_PERIOD:    return DIK_NUMPADPERIOD;
		case SDL_SCANCODE_F11:          return DIK_F11;
		case SDL_SCANCODE_F12:          return DIK_F12;
		case SDL_SCANCODE_KP_ENTER:     return DIK_NUMPADENTER;
		case SDL_SCANCODE_RCTRL:        return DIK_RCONTROL;
		case SDL_SCANCODE_KP_DIVIDE:    return DIK_NUMPADSLASH;
		case SDL_SCANCODE_PRINTSCREEN:  return DIK_SYSRQ;
		case SDL_SCANCODE_RALT:         return DIK_RALT;
		case SDL_SCANCODE_PAUSE:        return DIK_PAUSE;
		case SDL_SCANCODE_HOME:         return DIK_HOME;
		case SDL_SCANCODE_UP:           return DIK_UPARROW;
		case SDL_SCANCODE_PAGEUP:       return DIK_PGUP;
		case SDL_SCANCODE_LEFT:         return DIK_LEFTARROW;
		case SDL_SCANCODE_RIGHT:        return DIK_RIGHTARROW;
		case SDL_SCANCODE_END:          return DIK_END;
		case SDL_SCANCODE_DOWN:         return DIK_DOWNARROW;
		case SDL_SCANCODE_PAGEDOWN:     return DIK_PGDN;
		case SDL_SCANCODE_INSERT:       return DIK_INSERT;
		case SDL_SCANCODE_DELETE:       return DIK_DELETE;
		case SDL_SCANCODE_LGUI:         return DIK_LWIN;
		case SDL_SCANCODE_RGUI:         return DIK_RWIN;
		case SDL_SCANCODE_APPLICATION:  return DIK_APPS;
		default:                        return 0;
	}
}
