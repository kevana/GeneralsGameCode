// Compatibility stub: ddraw.h for non-Windows builds.
// Only the constants used by ddsfile.cpp are defined here.
#pragma once

// DDSCAPS2 flags used when loading DDS cube/volume textures
#ifndef DDSCAPS2_CUBEMAP
#define DDSCAPS2_CUBEMAP  0x00000200
#define DDSCAPS2_VOLUME   0x00200000
#endif
