/*
**	Command & Conquer Generals / Zero Hour
**	Cross-platform OpenGL header wrapper.
**	Provides a unified include path for OpenGL 3.3 core profile across platforms.
*/

#ifndef GL_COMPAT_H
#define GL_COMPAT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
// Future: Linux support via glad or system GL headers
#include <GL/gl.h>
#endif

#endif // GL_COMPAT_H
