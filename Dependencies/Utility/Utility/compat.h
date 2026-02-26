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

// GetDoubleClickTime — Windows API stub (returns 500ms default)
#ifndef GetDoubleClickTime
inline unsigned int GetDoubleClickTime() { return 500; }
#endif

// VK_RETURN — Windows virtual key code
#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif

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

// LARGE_INTEGER / QueryPerformanceCounter / QueryPerformanceFrequency / Sleep
#ifndef LARGE_INTEGER
#include <mach/mach_time.h>
typedef union _LARGE_INTEGER {
    struct { unsigned long LowPart; long HighPart; };
    long long QuadPart;
} LARGE_INTEGER;
inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* freq)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    // Convert mach_absolute_time to "ticks per second"
    freq->QuadPart = (long long)(1000000000LL * info.denom / info.numer);
    return TRUE;
}
inline BOOL QueryPerformanceCounter(LARGE_INTEGER* count)
{
    count->QuadPart = (long long)mach_absolute_time();
    return TRUE;
}
#endif
#ifndef Sleep
#include <unistd.h>
inline void Sleep(DWORD dwMilliseconds) { usleep(dwMilliseconds * 1000); }
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

// MSVC-specific min/max aliases
#ifndef __max
#define __max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef __min
#define __min(a, b) ((a) < (b) ? (a) : (b))
#endif

// Windows BOOL / TRUE / FALSE
#include <stdio.h>
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

// _access — Windows file-access check stub
#include <unistd.h>
#define _access access

// CreateDirectory(path, secAttr) — Windows API; secAttr ignored on POSIX
#include <sys/stat.h>
inline BOOL CreateDirectory(const char* path, void* /*securityAttrs*/)
{
    return mkdir(path, 0755) == 0 ? TRUE : FALSE;
}

// SIZE_T (Windows type alias for size_t)
#ifndef SIZE_T
typedef size_t SIZE_T;
#endif

// MEMORYSTATUS / GlobalMemoryStatus — Windows memory info API stub
#ifndef MEMORYSTATUS
typedef struct _MEMORYSTATUS {
    DWORD  dwLength;
    DWORD  dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS;
inline void GlobalMemoryStatus(MEMORYSTATUS* lpmst)
{
    if (!lpmst) return;
    memset(lpmst, 0, sizeof(*lpmst));
    lpmst->dwLength = sizeof(*lpmst);
}
#endif

// CopyFile — Windows API stub using POSIX I/O
inline BOOL CopyFile(const char* src, const char* dst, BOOL failIfExists)
{
    if (failIfExists) {
        FILE* f = fopen(dst, "rb");
        if (f) { fclose(f); return FALSE; }
    }
    FILE* in = fopen(src, "rb");
    if (!in) return FALSE;
    FILE* out = fopen(dst, "wb");
    if (!out) { fclose(in); return FALSE; }
    char buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) { fclose(in); fclose(out); return FALSE; }
    }
    fclose(in);
    fclose(out);
    return TRUE;
}

// DeleteFile — Windows API stub (returns nonzero on success)
inline BOOL DeleteFile(const char* path) { return remove(path) == 0; }

// GetLastError / FormatMessageW — Windows error reporting stubs
#include <errno.h>
inline DWORD GetLastError() { return (DWORD)errno; }
#ifndef FORMAT_MESSAGE_FROM_SYSTEM
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#endif
#include <wchar.h>
inline DWORD FormatMessageW(DWORD /*flags*/, const void* /*src*/, DWORD /*id*/,
    DWORD /*langId*/, wchar_t* buf, DWORD sz, void* /*args*/)
{
    if (buf && sz > 0) buf[0] = L'\0';
    return 0;
}
inline DWORD FormatMessage(DWORD /*flags*/, const void* /*src*/, DWORD /*id*/,
    DWORD /*langId*/, char* buf, DWORD sz, void* /*args*/)
{
    if (buf && sz > 0) buf[0] = '\0';
    return 0;
}

// SHGetSpecialFolderLocation / SHGetPathFromIDList / CSIDL — Windows Shell API stubs
typedef void* LPITEMIDLIST;
#ifndef CSIDL_DESKTOPDIRECTORY
#define CSIDL_DESKTOPDIRECTORY 0x0010
#endif
inline long SHGetSpecialFolderLocation(void* /*hwnd*/, int /*csidl*/, LPITEMIDLIST* pidl) { if (pidl) *pidl = nullptr; return -1; }
inline BOOL SHGetPathFromIDList(LPITEMIDLIST /*pidl*/, char* pszPath) { if (pszPath) pszPath[0] = '\0'; return FALSE; }

// LOCALE_SYSTEM_DEFAULT / GetDateFormat — Windows locale API stubs
#ifndef LOCALE_SYSTEM_DEFAULT
#define LOCALE_SYSTEM_DEFAULT 0x0800
#endif
inline int GetDateFormat(DWORD /*locale*/, DWORD /*flags*/, const void* /*date*/,
    const char* format, char* buf, int bufSize)
{
    if (!buf || bufSize <= 0) return 0;
    time_t t = time(nullptr);
    struct tm* lt = localtime(&t);
    if (!lt) { buf[0] = '\0'; return 0; }
    const char* fmt = "%Y";
    if (format) {
        if (strcmp(format, "yyyy") == 0) fmt = "%Y";
        else if (strcmp(format, "MM") == 0) fmt = "%m";
        else if (strcmp(format, "dd") == 0) fmt = "%d";
    }
    return (int)strftime(buf, bufSize, fmt, lt);
}

// _spawnl / _P_NOWAIT — stubs (process spawning not supported on POSIX via this API)
#ifndef _P_NOWAIT
#define _P_NOWAIT 1
#endif
#include <stdarg.h>
inline intptr_t _spawnl(int /*mode*/, const char* /*path*/, const char* /*arg0*/, ...) { return -1; }

// OSVERSIONINFO / GetVersionEx — Windows version info stubs
#ifndef OSVERSIONINFO
typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    char szCSDVersion[128];
} OSVERSIONINFOA;
typedef OSVERSIONINFOA OSVERSIONINFO;
#define VER_PLATFORM_WIN32_WINDOWS 1
inline BOOL GetVersionEx(OSVERSIONINFO* lpVersionInfo)
{
    if (!lpVersionInfo) return FALSE;
    memset(lpVersionInfo, 0, sizeof(*lpVersionInfo));
    lpVersionInfo->dwMajorVersion = 10;  // Pretend modern OS
    lpVersionInfo->dwPlatformId = 2;     // VER_PLATFORM_WIN32_NT
    return TRUE;
}
#endif

#endif

