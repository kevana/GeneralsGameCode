/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 TheSuperHackers
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

// This file contains macros to help compiling on non-windows platforms.
#pragma once

#ifndef _WIN32
// For size_t, uintptr_t, ptrdiff_t
#include <cstddef>
#include <cstdint>
// For strlen, memset, memcpy, strcpy, etc.
#include <string.h>
// For isdigit
#include <cctype>

// __forceinline
#ifndef __forceinline
#if defined __has_attribute && __has_attribute(always_inline)
#define __forceinline __attribute__((always_inline)) inline
#else
#define __forceinline inline
#endif
#endif

// _cdecl / __cdecl
#ifndef _cdecl
#define _cdecl
#endif
#ifndef __cdecl
#define __cdecl
#endif

// OutputDebugString
#ifndef OutputDebugString
#define OutputDebugString(str) printf("%s\n", str)
#endif

// Windows primitive types
#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef BOOL
typedef int BOOL;
#endif

// SYSTEMTIME
#include <time.h>
#ifndef SYSTEMTIME
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
#endif

// _stat / _S_IFDIR - POSIX equivalents
#include <sys/stat.h>
#ifndef _stat
#define _stat stat
#endif
#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif

// GetCommandLineA - stub returning empty string on non-Windows
inline const char* GetCommandLineA() { return ""; }

// GetCurrentDirectory - POSIX equivalent
#include <unistd.h>
inline DWORD GetCurrentDirectoryA(DWORD size, char* buf)
{
    return getcwd(buf, size) ? (DWORD)strlen(buf) : 0;
}
#define GetCurrentDirectory GetCurrentDirectoryA

// GetFileAttributes - stub (returns INVALID_FILE_ATTRIBUTES if not found)
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#endif
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#endif
inline DWORD GetFileAttributesA(const char* path)
{
    struct stat st;
    if (stat(path, &st) != 0) return INVALID_FILE_ATTRIBUTES;
    return S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
}
#define GetFileAttributes GetFileAttributesA

// BITMAPFILEHEADER / BITMAPINFOHEADER (GDI types)
#ifndef BITMAPFILEHEADER
#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    long  biWidth;
    long  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    long  biXPelsPerMeter;
    long  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;
#define BI_RGB 0
#endif

// ZeroMemory / FillMemory / CopyMemory (Windows memory macros)
#include <string.h>
#ifndef ZeroMemory
#define ZeroMemory(p, s) memset((p), 0, (s))
#endif
#ifndef FillMemory
#define FillMemory(p, s, v) memset((p), (v), (s))
#endif
#ifndef CopyMemory
#define CopyMemory(d, s, n) memcpy((d), (s), (n))
#endif

// MSVC math function aliases
#include <cmath>
#ifndef _isnan
#define _isnan isnan
#endif
#ifndef _finite
#define _finite isfinite
#endif
#ifndef _isinf
#define _isinf isinf
#endif

// __int64 / _int64 (MSVC-specific signed 64-bit integer types).
// Use #define so that "unsigned __int64" expands to "unsigned long long".
#ifndef __int64
#define __int64 long long
#endif
#ifndef _int64
#define _int64 long long
#endif

// HANDLE (Windows generic handle type)
#ifndef HANDLE
typedef void* HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#endif

// _MAX_DRIVE, _MAX_DIR, _MAX_FNAME, _MAX_EXT, _MAX_PATH
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif
#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif

#include "mem_compat.h"
#include "string_compat.h"
#include "tchar_compat.h"
#include "wchar_compat.h"
#include "time_compat.h"
#include "thread_compat.h"
#include "socket_compat.h"

#endif

