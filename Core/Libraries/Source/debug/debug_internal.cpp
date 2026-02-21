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

/////////////////////////////////////////////////////////////////////////EA-V1
// $File: //depot/GeneralsMD/Staging/code/Libraries/Source/debug/debug_internal.cpp $
// $Author: mhoffe $
// $Revision: #1 $
// $DateTime: 2003/07/03 11:55:26 $
//
// (c) 2003 Electronic Arts
//
// Implementation of internal code
//////////////////////////////////////////////////////////////////////////////

#include "debug.h"

#ifdef _WIN32
#include <windows.h>

void DebugInternalAssert(const char *file, int line, const char *expr)
{
  char buf[512];
  wsprintf(buf,"File %s, line %i:\n%s",file,line,expr);
  MessageBox(nullptr,buf,"Internal assert failed",
                        MB_OK|MB_ICONSTOP|MB_TASKMODAL|MB_SETFOREGROUND);
  TerminateProcess(GetCurrentProcess(),666);
}

void *DebugAllocMemory(unsigned numBytes)
{
  HGLOBAL h=GlobalAlloc(GMEM_FIXED,numBytes);
  if (!h)
    DCRASH_RELEASE("Debug mem alloc failed");
  return (void *)h;
}

void *DebugReAllocMemory(void *oldPtr, unsigned newSize)
{
  if (!oldPtr)
    return newSize?DebugAllocMemory(newSize):nullptr;
  if (!newSize)
  {
    GlobalFree((HGLOBAL)oldPtr);
    return nullptr;
  }
  HGLOBAL h=GlobalReAlloc((HGLOBAL)oldPtr,newSize,0);
  if (!h)
  {
    h=GlobalAlloc(GMEM_FIXED,newSize);
    if (!h)
      DCRASH_RELEASE("Debug mem realloc failed");
    unsigned oldSize=GlobalSize((HGLOBAL)oldPtr);
    memcpy((void *)h,oldPtr,oldSize<newSize?oldSize:newSize);
    GlobalFree((HGLOBAL)oldPtr);
  }
  return (void *)h;
}

void DebugFreeMemory(void *ptr)
{
  if (ptr)
    GlobalFree((HGLOBAL)ptr);
}

#else // !_WIN32

#include <cstdio>
#include <cstdlib>
#include <cstring>

void DebugInternalAssert(const char *file, int line, const char *expr)
{
  fprintf(stderr, "Internal assert failed - File %s, line %i:\n%s\n", file, line, expr);
  abort();
}

void *DebugAllocMemory(unsigned numBytes)
{
  void *p = malloc(numBytes);
  if (!p)
    DCRASH_RELEASE("Debug mem alloc failed");
  return p;
}

void *DebugReAllocMemory(void *oldPtr, unsigned newSize)
{
  if (!oldPtr)
    return newSize ? DebugAllocMemory(newSize) : nullptr;
  if (!newSize)
  {
    free(oldPtr);
    return nullptr;
  }
  void *p = realloc(oldPtr, newSize);
  if (!p)
    DCRASH_RELEASE("Debug mem realloc failed");
  return p;
}

void DebugFreeMemory(void *ptr)
{
  free(ptr);
}

#endif // _WIN32
