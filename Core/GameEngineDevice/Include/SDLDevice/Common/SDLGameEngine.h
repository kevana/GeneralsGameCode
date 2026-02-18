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

// FILE: SDLGameEngine.h ///////////////////////////////////////////////////////
// SDL2-based platform engine for macOS (and other non-Windows platforms)
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common/GameEngine.h"
#include "StdDevice/Common/StdBIGFileSystem.h"
#include "StdDevice/Common/StdLocalFileSystem.h"
#include "GameLogic/GameLogic.h"
#include "GameNetwork/NetworkInterface.h"

// W3D device classes shared across platforms
#include "W3DDevice/Common/W3DModuleFactory.h"
#include "W3DDevice/Common/W3DFunctionLexicon.h"
#include "W3DDevice/Common/W3DRadar.h"
#include "W3DDevice/Common/W3DThingFactory.h"
#include "W3DDevice/GameLogic/W3DGameLogic.h"
#include "W3DDevice/GameClient/W3DGameClient.h"

struct SDL_Window;

//-------------------------------------------------------------------------------------------------
/** SDL2-based game engine for macOS and other non-Windows platforms */
//-------------------------------------------------------------------------------------------------
class SDLGameEngine : public GameEngine
{

public:

	SDLGameEngine();
	virtual ~SDLGameEngine();

	virtual void init( void );                              ///< initialization
	virtual void reset( void );                             ///< reset engine
	virtual void update( void );                            ///< update the game engine
	virtual void serviceWindowsOS( void );                  ///< poll SDL events (name kept for interface compat)

	SDL_Window *getWindow( void ) const { return m_window; }

protected:

	virtual GameLogic *createGameLogic( void );             ///< factory for game logic
	virtual GameClient *createGameClient( void );           ///< factory for game client
	virtual ModuleFactory *createModuleFactory( void );     ///< factory for creating modules
	virtual ThingFactory *createThingFactory( void );       ///< factory for the thing factory
	virtual FunctionLexicon *createFunctionLexicon( void ); ///< factory for function lexicon
	virtual LocalFileSystem *createLocalFileSystem( void ); ///< factory for local file system
	virtual ArchiveFileSystem *createArchiveFileSystem( void ); ///< factory for archive file system
	virtual NetworkInterface *createNetwork( void );        ///< Factory for the network
	virtual Radar *createRadar( void );                     ///< Factory for radar
	virtual WebBrowser *createWebBrowser( void );           ///< Factory for embedded browser
	virtual AudioManager *createAudioManager( void );       ///< Factory for audio device
	virtual ParticleSystemManager* createParticleSystemManager( void );


protected:
	SDL_Window *m_window;
};

// INLINE -----------------------------------------------------------------------------------------
inline GameLogic *SDLGameEngine::createGameLogic( void ) { return NEW W3DGameLogic; }
inline GameClient *SDLGameEngine::createGameClient( void ) { return NEW W3DGameClient; }
inline ModuleFactory *SDLGameEngine::createModuleFactory( void ) { return NEW W3DModuleFactory; }
inline ThingFactory *SDLGameEngine::createThingFactory( void ) { return NEW W3DThingFactory; }
inline FunctionLexicon *SDLGameEngine::createFunctionLexicon( void ) { return NEW W3DFunctionLexicon; }
inline LocalFileSystem *SDLGameEngine::createLocalFileSystem( void ) { return NEW StdLocalFileSystem; }
inline ArchiveFileSystem *SDLGameEngine::createArchiveFileSystem( void ) { return NEW StdBIGFileSystem; }

inline NetworkInterface *SDLGameEngine::createNetwork( void ) { return NetworkInterface::createNetwork(); }
inline Radar *SDLGameEngine::createRadar( void ) { return NEW W3DRadar; }
