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

// This file contains the time functions for compatibility with non-windows platforms.
#pragma once
#include <time.h>

#define TIMERR_NOERROR 0
typedef int MMRESULT;
static inline MMRESULT timeBeginPeriod(int) { return TIMERR_NOERROR; }
static inline MMRESULT timeEndPeriod(int) { return TIMERR_NOERROR; }

inline unsigned int timeGetTime()
{
  struct timespec ts;
#ifdef CLOCK_BOOTTIME
  clock_gettime(CLOCK_BOOTTIME, &ts);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts); // macOS: CLOCK_BOOTTIME is Linux-only
#endif
  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
inline unsigned int GetTickCount()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  // Return ms since boot
  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// SYSTEMTIME is defined in compat.h; GetLocalTime fills it from localtime()
inline void GetLocalTime(SYSTEMTIME* st)
{
  time_t t = time(nullptr);
  struct tm* lt = localtime(&t);
  if (!st || !lt) return;
  st->wYear         = (unsigned short)(lt->tm_year + 1900);
  st->wMonth        = (unsigned short)(lt->tm_mon + 1);
  st->wDayOfWeek    = (unsigned short)(lt->tm_wday);
  st->wDay          = (unsigned short)(lt->tm_mday);
  st->wHour         = (unsigned short)(lt->tm_hour);
  st->wMinute       = (unsigned short)(lt->tm_min);
  st->wSecond       = (unsigned short)(lt->tm_sec);
  st->wMilliseconds = 0;
}

