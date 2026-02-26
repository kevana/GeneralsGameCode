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

// This file contains string macros and alias functions to help compiling on non-windows platforms
#pragma once
#include <ctype.h>

typedef const char* LPCSTR;
typedef char* LPSTR;

// String functions — declared with C linkage to match GameSpy's extern "C" declarations.
#ifdef __cplusplus
extern "C" {
#endif
inline char *_strlwr(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}

inline char *strupr(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = toupper(str[i]);
  }
  return str;
}
#ifdef __cplusplus
}
#endif

#include <wctype.h>  // towlower

#define strlwr _strlwr
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

// MSVC uses underscore-prefixed names for these
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _strupr strupr  // strupr is defined above

// POSIX has strdup, but not _strdup (Windows name)
#include <string.h>
#ifndef _strdup
#define _strdup strdup
#endif

// lstrcpyn / lstrcat are Windows APIs.
// Provided as inline functions so ::lstrcpyn(...) and ::lstrcat(...) are valid.
#include <stddef.h>
inline char* lstrcpyn(char* dst, const char* src, size_t n)
{
    if (n > 0) { strncpy(dst, src, n); dst[n - 1] = '\0'; }
    return dst;
}

inline char* lstrcat(char* dst, const char* src)
{
    return strcat(dst, src);
}

inline char* lstrcpy(char* dst, const char* src)
{
    return strcpy(dst, src);
}

inline int lstrlen(const char* str)
{
    return static_cast<int>(strlen(str));
}

inline int lstrcmpi(const char* s1, const char* s2)
{
    return strcasecmp(s1, s2);
}

// itoa — MSVC non-standard integer-to-string (base 2-36)
#include <stdio.h>
#ifndef _itoa
inline char* itoa(int value, char* str, int base)
{
    if (base == 10) { sprintf(str, "%d", value); return str; }
    // Handle other bases via stdlib
    char tmp[65]; int i = 0; unsigned int uval = (unsigned int)value;
    if (base == 10 && value < 0) { str[i++] = '-'; uval = (unsigned int)(-value); }
    int start = i;
    do { tmp[i++] = "0123456789abcdefghijklmnopqrstuvwxyz"[uval % base]; uval /= base; } while (uval);
    tmp[i] = '\0';
    // reverse
    for (int a = start, b = i - 1; a < b; ++a, --b) { char c = tmp[a]; tmp[a] = tmp[b]; tmp[b] = c; }
    strcpy(str, tmp);
    return str;
}
#define _itoa itoa
#endif

