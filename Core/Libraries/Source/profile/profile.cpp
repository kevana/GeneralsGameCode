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
// $File: //depot/GeneralsMD/Staging/code/Libraries/Source/profile/profile.cpp $
// $Author: mhoffe $
// $Revision: #6 $
// $DateTime: 2003/08/14 13:43:29 $
//
// (c) 2003 Electronic Arts
//
// Profile module main code
//////////////////////////////////////////////////////////////////////////////

#include "profile.h"
#include "internal.h"
#include <new>
#ifdef _WIN32
#include "mmsystem.h"
#else
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <chrono>
#endif

// yuk, I'm doing this so weird because the destructor
// of cmd must never be called...
static ProfileCmdInterface &cmd=*(ProfileCmdInterface *)new
                  (ProfileAllocMemory(sizeof(ProfileCmdInterface))) ProfileCmdInterface();

// we have this here so that our command interface will always
// be linked in as well...
static bool __RegisterDebugCmdGroup_Profile=Debug::AddCommands("profile",&cmd);

#ifdef _WIN32
void *ProfileAllocMemory(unsigned numBytes)
{
  HGLOBAL h=GlobalAlloc(GMEM_FIXED,numBytes);
  if (!h)
    DCRASH_RELEASE("Debug mem alloc failed");
  return (void *)h;
}

void *ProfileReAllocMemory(void *oldPtr, unsigned newSize)
{
  if (!oldPtr)
    return newSize?ProfileAllocMemory(newSize):nullptr;
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

void ProfileFreeMemory(void *ptr)
{
  if (ptr)
    GlobalFree((HGLOBAL)ptr);
}
#else // !_WIN32
void *ProfileAllocMemory(unsigned numBytes)
{
  void *p = malloc(numBytes);
  if (!p)
    DCRASH_RELEASE("Debug mem alloc failed");
  return p;
}

void *ProfileReAllocMemory(void *oldPtr, unsigned newSize)
{
  if (!oldPtr)
    return newSize ? ProfileAllocMemory(newSize) : nullptr;
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

void ProfileFreeMemory(void *ptr)
{
  free(ptr);
}
#endif // _WIN32

//////////////////////////////////////////////////////////////////////////////

static _int64 GetClockCyclesFast(void)
{
  // this is where we're adding our internal result functions
  Profile::AddResultFunction(ProfileResultFileCSV::Create,
                              "file_csv",
                              "");
  Profile::AddResultFunction(ProfileResultFileCSV::Create,
                              "file_dot",
                              "[ file [ frame_name [ fold_threshold ] ] ]");

#ifdef _WIN32
  // measure clock cycles 3 times for 20 msec each
  // then take the 2 counts that are closest, average
  _int64 n[3];
  for (int k=0;k<3;k++)
  {
    unsigned timeEnd=timeGetTime()+2;
    while (timeGetTime()<timeEnd);

    _int64 start,startQPC,endQPC;
    QueryPerformanceCounter((LARGE_INTEGER *)&startQPC);
    ProfileGetTime(start);
    timeEnd+=20;
    while (timeGetTime()<timeEnd);
    ProfileGetTime(n[k]);
    n[k]-=start;

    if (QueryPerformanceCounter((LARGE_INTEGER *)&endQPC))
    {
      _int64 freq;
      QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
      n[k]=(n[k]*freq)/(endQPC-startQPC);
    }
    else
    {
      n[k]=(n[k]*1000)/20;
    }
  }

  _int64 d01=n[1]-n[0],d02=n[2]-n[0],d12=n[2]-n[1];
  if (d01<0) d01=-d01;
  if (d02<0) d02=-d02;
  if (d12<0) d12=-d12;
  _int64 avg;
  if (d01<d02)
    avg=d01<d12?n[0]+n[1]:n[1]+n[2];
  else
    avg=d02<d12?n[0]+n[2]:n[1]+n[2];

  return ((avg/2+500000)/1000000)*1000000;
#else
  // On non-Windows, use std::chrono to estimate CPU frequency
  using clock = std::chrono::high_resolution_clock;
  auto start = clock::now();
  _int64 tscStart;
  ProfileGetTime(tscStart);

  // Busy-wait for ~20ms
  while (std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count() < 20)
    ;

  _int64 tscEnd;
  ProfileGetTime(tscEnd);
  auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start).count();

  if (elapsed > 0)
  {
    _int64 cyclesPerSec = (tscEnd - tscStart) * 1000000000LL / elapsed;
    return ((cyclesPerSec + 500000) / 1000000) * 1000000;
  }
  return 1000000000LL; // 1 GHz fallback
#endif
}

unsigned Profile::m_rec;
char **Profile::m_recNames;
unsigned Profile::m_names;
Profile::FrameName *Profile::m_frameNames;
_int64 Profile::m_clockCycles=GetClockCyclesFast();
Profile::PatternListEntry *Profile::firstPatternEntry;
Profile::PatternListEntry *Profile::lastPatternEntry;

void Profile::StartRange(const char *range)
{
  // set default
  if (!range)
    range="frame";

  // known name?
  unsigned k=0;
  for (;k<m_names;++k)
    if (strcmp(range,m_frameNames[k].name) == 0)
      break;
  if (k==m_names)
  {
    // no, must add to list
    m_frameNames=(FrameName *)ProfileReAllocMemory(m_frameNames,(++m_names)*sizeof(FrameName));
    m_frameNames[k].name=(char *)ProfileAllocMemory(strlen(range)+1);
    strcpy(m_frameNames[k].name,range);
    m_frameNames[k].frames=0;
    m_frameNames[k].isRecording=false;
    m_frameNames[k].doAppend=false;
    m_frameNames[k].lastGlobalIndex=-1;
  }

  // stop old recording?
  if (m_frameNames[k].isRecording)
    StopRange(range);

  // start new recording
  m_frameNames[k].isRecording=true;
  m_frameNames[k].doAppend=false;

  // but check first: is recording enabled?
  bool active=false;
  for (PatternListEntry *cur=firstPatternEntry;cur;cur=cur->next)
  {
    if (SimpleMatch(range,cur->pattern))
      active=cur->isActive;
  }

  if (active)
  {
#ifdef RTS_PROFILE
    m_frameNames[k].funcIndex=ProfileFuncLevelTracer::FrameStart();
    DASSERT(m_frameNames[k].funcIndex>=0);
#endif
    m_frameNames[k].highIndex=ProfileId::FrameStart();
    DASSERT(m_frameNames[k].highIndex>=0);
  }
  else
  {
    m_frameNames[k].funcIndex=-1;
    m_frameNames[k].highIndex=-1;
  }
}

void Profile::AppendRange(const char *range)
{
  // set default
  if (!range)
    range="frame";

  // known name?
  unsigned k=0;
  for (;k<m_names;++k)
    if (strcmp(range,m_frameNames[k].name) == 0)
      break;
  if (k==m_names)
  {
    // no, so StartRange will do the job for us
    StartRange(range);
    return;
  }

  // still recording?
  if (m_frameNames[k].isRecording)
    // don't do anything
    return;

  // start new recording
  m_frameNames[k].isRecording=true;
  m_frameNames[k].doAppend=true;

  // but check first: is recording enabled?
  bool active=false;
  for (PatternListEntry *cur=firstPatternEntry;cur;cur=cur->next)
  {
    if (SimpleMatch(range,cur->pattern))
      active=cur->isActive;
  }

  if (active)
  {
#ifdef RTS_PROFILE
    m_frameNames[k].funcIndex=ProfileFuncLevelTracer::FrameStart();
    DASSERT(m_frameNames[k].funcIndex>=0);
#endif
    m_frameNames[k].highIndex=ProfileId::FrameStart();
    DASSERT(m_frameNames[k].highIndex>=0);
  }
  else
  {
    m_frameNames[k].funcIndex=-1;
    m_frameNames[k].highIndex=-1;
  }
}

void Profile::StopRange(const char *range)
{
  // set default
  if (!range)
    range="frame";

  // known name?
  unsigned k=0;
  for (;k<m_names;++k)
    if (strcmp(range,m_frameNames[k].name) == 0)
      break;
  DFAIL_IF(k==m_names) return;
  DFAIL_IF(!m_frameNames[k].isRecording) return;

  // stop recording
  m_frameNames[k].isRecording=false;
  if (
#ifdef RTS_PROFILE
    m_frameNames[k].funcIndex>=0 ||
#endif
    m_frameNames[k].highIndex>=0
    )
  {
    // add to list of known frames?
    int atIndex;
    if (!m_frameNames[k].doAppend||
        m_frameNames[k].lastGlobalIndex<0)
    {
      atIndex=-1;
      m_frameNames[k].lastGlobalIndex=m_rec;
      m_recNames=(char **)ProfileReAllocMemory(m_recNames,(m_rec+1)*sizeof(char *));
      m_recNames[m_rec]=(char *)ProfileAllocMemory(strlen(range)+1+6);
      sprintf(m_recNames[m_rec++],"%s:%i",range,++m_frameNames[k].frames);
    }
    else
      atIndex=m_frameNames[k].lastGlobalIndex;
#ifdef RTS_PROFILE
    if (m_frameNames[k].funcIndex>=0)
      ProfileFuncLevelTracer::FrameEnd(m_frameNames[k].funcIndex,atIndex);
    if (m_frameNames[k].highIndex>=0)
#endif
      ProfileId::FrameEnd(m_frameNames[k].highIndex,atIndex);
  }
}

bool Profile::IsEnabled(void)
{
  for (unsigned k=0;k<m_names;++k)
    if (m_frameNames[k].isRecording)
      return true;
  return false;
}

unsigned Profile::GetFrameCount(void)
{
  return m_rec;
}

const char *Profile::GetFrameName(unsigned frame)
{
  return frame>=m_rec?nullptr:m_recNames[frame];
}

void Profile::ClearTotals(void)
{
#ifdef RTS_PROFILE
  ProfileFuncLevelTracer::ClearTotals();
#endif
  ProfileId::ClearTotals();
}

_int64 Profile::GetClockCyclesPerSecond(void)
{
  return m_clockCycles;
}

void Profile::AddResultFunction(ProfileResultInterface* (*func)(int, const char * const *),
                                const char *name, const char *arg)
{
  ProfileCmdInterface::AddResultFunction(func,name,arg);
}

bool Profile::SimpleMatch(const char *str, const char *pattern)
{
  DASSERT(str);
  DASSERT(pattern);
  while (*str&&*pattern)
  {
    if (*pattern=='*')
    {
      pattern++;
      while (*str)
        if (SimpleMatch(str++,pattern))
          return true;
      return *str==*pattern;
    }
    else
    {
      if (*str++!=*pattern++)
        return false;
    }
  }

  return *str==*pattern;
}

static void ProfileShutdown(void)
{
#ifdef RTS_PROFILE
  ProfileFuncLevelTracer::Shutdown();
#endif
  ProfileId::Shutdown();

  DLOG("CPU speed is " << unsigned(Profile::GetClockCyclesPerSecond()) << " Hz.\n");

  cmd.RunResultFunctions();
}

int profileTracerInit=atexit(ProfileShutdown);
