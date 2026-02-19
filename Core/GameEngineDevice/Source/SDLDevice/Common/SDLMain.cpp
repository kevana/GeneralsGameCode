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
//  SDL2 Entry Point for macOS (and other non-Windows platforms)               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// FILE: SDLMain.cpp ///////////////////////////////////////////////////////////
// Portable entry point using SDL2, replaces WinMain.cpp on non-Windows
////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"

#include <SDL.h>
#include <cstdio>
#include <cstring>

#include "Common/GameEngine.h"
#include "Common/GlobalData.h"
#include "Common/GameMemory.h"
#include "Common/CommandLine.h"
#include "SDLDevice/Common/SDLGameEngine.h"

// The game engine singleton — defined in GameEngine.cpp
extern GameEngine *TheGameEngine;

// GameMain is defined in GameMain.cpp
extern Int GameMain( void );

// Global window handle, equivalent to Win32's HWND ApplicationHWnd
// On SDL, this points to the SDL_Window. Referenced by W3DDisplay etc.
HWND ApplicationHWnd = nullptr;

// The SDL game engine instance (analogous to TheWin32Mouse etc.)
static SDLGameEngine *TheSDLEngine = nullptr;
static bool isSDLMainActive = true;

//-------------------------------------------------------------------------------------------------
/** CreateGameEngine - factory function called by GameMain() */
//-------------------------------------------------------------------------------------------------
GameEngine *CreateGameEngine( void )
{
	SDLGameEngine *engine = NEW SDLGameEngine;
	engine->setIsActive(isSDLMainActive);
	TheSDLEngine = engine;
	return engine;
}

//-------------------------------------------------------------------------------------------------
/** Main entry point */
//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	// Initialize memory manager
	InitMemoryManager();

	// Build a command line string from argc/argv for the existing parser
	// (The game's command line parser expects a single string)
	char cmdLine[4096] = {0};
	for (int i = 1; i < argc; i++)
	{
		if (i > 1)
			strcat(cmdLine, " ");
		strcat(cmdLine, argv[i]);
	}

	// Parse command line
	CommandLine::parseCommandLineForStartup();

	int exitcode = GameMain();

	// Cleanup
	ShutdownMemoryManager();
	SDL_Quit();

	return exitcode;
}
