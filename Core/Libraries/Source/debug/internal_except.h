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
// $File: //depot/GeneralsMD/Staging/code/Libraries/Source/debug/internal_except.h $
// $Author: mhoffe $
// $Revision: #1 $
// $DateTime: 2003/07/03 11:55:26 $
//
// (c) 2003 Electronic Arts
//
// Unhandled exception handler
//////////////////////////////////////////////////////////////////////////////

#pragma once

/// \internal exception handler
class DebugExceptionhandler
{
  DebugExceptionhandler(const DebugExceptionhandler&);
  DebugExceptionhandler& operator=(const DebugExceptionhandler&);

  DebugExceptionhandler(void);
  ~DebugExceptionhandler();

#ifdef _WIN32
  static void LogExceptionLocation(Debug &dbg, struct _EXCEPTION_POINTERS *exptr);
  static void LogRegisters(Debug &dbg, struct _EXCEPTION_POINTERS *exptr);
  static void LogFPURegisters(Debug &dbg, struct _EXCEPTION_POINTERS *exptr);

public:
  static const char *GetExceptionType(struct _EXCEPTION_POINTERS *exptr, char *explanation);
  static long __stdcall ExceptionFilter(struct _EXCEPTION_POINTERS* pExPtrs);
#else
public:
#endif
};
