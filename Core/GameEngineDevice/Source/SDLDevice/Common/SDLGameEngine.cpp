/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  macOS / SDL2 Game Engine Implementation                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// FILE: SDLGameEngine.cpp /////////////////////////////////////////////////////
// SDL2-based platform engine for macOS (and other non-Windows platforms)
////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"

#include <SDL.h>

#include "SDLDevice/Common/SDLGameEngine.h"
#include "SDLDevice/GameClient/SDLKeyboard.h"
#include "SDLDevice/GameClient/SDLMouse.h"
#include "Common/GameAudio.h"
#include "Common/PerfTimer.h"
#include "GameClient/Keyboard.h"
#include "GameClient/Mouse.h"
#include "GameNetwork/LANAPICallbacks.h"
#include "W3DDevice/GameClient/W3DParticleSys.h"

//-------------------------------------------------------------------------------------------------
/** Constructor for SDLGameEngine */
//-------------------------------------------------------------------------------------------------
SDLGameEngine::SDLGameEngine()
	: m_window(nullptr)
{
}

//-------------------------------------------------------------------------------------------------
/** Destructor for SDLGameEngine */
//-------------------------------------------------------------------------------------------------
SDLGameEngine::~SDLGameEngine()
{
	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
	SDL_Quit();
}

//-------------------------------------------------------------------------------------------------
/** Initialize the game engine */
//-------------------------------------------------------------------------------------------------
void SDLGameEngine::init( void )
{
	// Set OpenGL attributes before window creation
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Create the game window with OpenGL support
	m_window = SDL_CreateWindow(
		"Command & Conquer Generals",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (m_window)
	{
		// Set the global ApplicationHWnd so W3DDisplay can pass it to WW3D::Init
		extern HWND ApplicationHWnd;
		ApplicationHWnd = (HWND)m_window;
	}

	// extending functionality
	GameEngine::init();
}

//-------------------------------------------------------------------------------------------------
/** Reset the system */
//-------------------------------------------------------------------------------------------------
void SDLGameEngine::reset( void )
{
	// extending functionality
	GameEngine::reset();
}

//-------------------------------------------------------------------------------------------------
/** Update the game engine */
//-------------------------------------------------------------------------------------------------
void SDLGameEngine::update( void )
{
	// call the engine normal update
	GameEngine::update();

	// allow SDL to process events
	serviceWindowsOS();
}

//-------------------------------------------------------------------------------------------------
/** Poll and dispatch SDL events. Named serviceWindowsOS for interface
 *  compatibility with the base GameEngine class. */
//-------------------------------------------------------------------------------------------------
void SDLGameEngine::serviceWindowsOS( void )
{
	// Get typed pointers to SDL input devices (if available)
	SDLKeyboard *sdlKeyboard = dynamic_cast<SDLKeyboard*>(TheKeyboard);
	SDLMouse *sdlMouse = dynamic_cast<SDLMouse*>(TheMouse);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			setQuitting(true);
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				setIsActive(true);
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				setIsActive(false);
				break;
			}
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (sdlKeyboard) {
				sdlKeyboard->pushKeyEvent(event.key);
			}
			break;

		case SDL_MOUSEMOTION:
			if (sdlMouse) {
				sdlMouse->pushMouseMotion(event.motion);
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (sdlMouse) {
				sdlMouse->pushMouseButton(event.button);
			}
			break;

		case SDL_MOUSEWHEEL:
			if (sdlMouse) {
				sdlMouse->pushMouseWheel(event.wheel);
			}
			break;

		default:
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
/** Factory stubs for subsystems not yet ported */
//-------------------------------------------------------------------------------------------------
WebBrowser *SDLGameEngine::createWebBrowser( void )
{
	// Web browser not supported on macOS
	return nullptr;
}

AudioManager *SDLGameEngine::createAudioManager( void )
{
	// Use dummy audio manager until OpenAL implementation is complete
	return NEW AudioManagerDummy;
}

ParticleSystemManager* SDLGameEngine::createParticleSystemManager( void )
{
	return NEW W3DParticleSystemManager;
}
