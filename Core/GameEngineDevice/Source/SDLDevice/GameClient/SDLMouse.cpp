/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
*/

// FILE: SDLMouse.cpp //////////////////////////////////////////////////////////
// SDL2-based mouse implementation for non-Windows platforms.
// Translates SDL mouse events to the engine's MouseIO structure.
////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"
#include "SDLDevice/GameClient/SDLMouse.h"

SDLMouse::SDLMouse()
	: m_eventIndex(0)
{
}

SDLMouse::~SDLMouse()
{
}

void SDLMouse::init(void)
{
	Mouse::init();
	m_eventQueue.clear();
	m_eventIndex = 0;

	m_numButtons = 3;
	m_numAxes = 2;
	m_forceFeedback = false;
	m_inputMovesAbsolute = true;
}

void SDLMouse::update(void)
{
	// Get current absolute mouse position from SDL
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	moveMouse(mx, my, MOUSE_MOVE_ABSOLUTE);

	// Reset event index for this frame
	m_eventIndex = 0;

	// Let base class process events (calls getMouseEvent)
	Mouse::update();

	// Clear consumed events
	m_eventQueue.clear();
	m_eventIndex = 0;
}

void SDLMouse::initCursorResources(void)
{
	// Use default SDL cursor for now
}

void SDLMouse::setCursor(MouseCursor cursor)
{
	m_currentCursor = cursor;
	// SDL cursor switching could be implemented here
}

void SDLMouse::capture(void)
{
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void SDLMouse::releaseCapture(void)
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void SDLMouse::pushMouseMotion(const SDL_MouseMotionEvent& event)
{
	SDLMouseEvent me = {};
	me.pos.x = event.x;
	me.pos.y = event.y;
	me.delta.x = event.xrel;
	me.delta.y = event.yrel;
	me.wheelPos = 0;
	me.leftState = MBS_None;
	me.rightState = MBS_None;
	me.middleState = MBS_None;
	me.time = event.timestamp;
	m_eventQueue.push_back(me);
}

void SDLMouse::pushMouseButton(const SDL_MouseButtonEvent& event)
{
	SDLMouseEvent me = {};
	me.pos.x = event.x;
	me.pos.y = event.y;
	me.delta.x = 0;
	me.delta.y = 0;
	me.wheelPos = 0;
	me.leftState = MBS_None;
	me.rightState = MBS_None;
	me.middleState = MBS_None;
	me.time = event.timestamp;

	MouseButtonState state;
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		state = (event.clicks >= 2) ? MBS_DoubleClick : MBS_Down;
	} else {
		state = MBS_Up;
	}

	switch (event.button) {
		case SDL_BUTTON_LEFT:   me.leftState = state; break;
		case SDL_BUTTON_RIGHT:  me.rightState = state; break;
		case SDL_BUTTON_MIDDLE: me.middleState = state; break;
	}

	m_eventQueue.push_back(me);
}

void SDLMouse::pushMouseWheel(const SDL_MouseWheelEvent& event)
{
	SDLMouseEvent me = {};
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	me.pos.x = mx;
	me.pos.y = my;
	me.delta.x = 0;
	me.delta.y = 0;
	me.wheelPos = event.y * MOUSE_WHEEL_DELTA;  // Convert to DirectInput-style delta
	me.leftState = MBS_None;
	me.rightState = MBS_None;
	me.middleState = MBS_None;
	me.time = event.timestamp;
	m_eventQueue.push_back(me);
}

UnsignedByte SDLMouse::getMouseEvent(MouseIO *result, Bool flush)
{
	if (m_eventIndex >= m_eventQueue.size()) {
		return MOUSE_NONE;
	}

	const SDLMouseEvent& ev = m_eventQueue[m_eventIndex++];
	result->pos = ev.pos;
	result->deltaPos = ev.delta;
	result->wheelPos = ev.wheelPos;
	result->leftState = ev.leftState;
	result->leftEvent = (ev.leftState != MBS_None) ? 1 : MOUSE_EVENT_NONE;
	result->rightState = ev.rightState;
	result->rightEvent = (ev.rightState != MBS_None) ? 1 : MOUSE_EVENT_NONE;
	result->middleState = ev.middleState;
	result->middleEvent = (ev.middleState != MBS_None) ? 1 : MOUSE_EVENT_NONE;
	result->time = ev.time;

	return MOUSE_OK;
}
