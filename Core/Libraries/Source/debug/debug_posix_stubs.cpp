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

// Minimal non-Windows implementation of the Debug class.
// On Windows, the full debug library provides MessageBox-based asserts,
// file/network/console I/O, stack walking, and exception handling.
// On non-Windows, we provide a minimal stderr-based implementation.

#ifndef _WIN32

#include "debug.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

// Static members
Debug Debug::Instance;
void *Debug::PreStatic = nullptr;
void *Debug::PostStatic = nullptr;
unsigned Debug::curStackFrame = 0;

// Constructor / Destructor
Debug::Debug(void)
{
	memset(ioBuffer, 0, sizeof(ioBuffer));
	curType = DebugIOInterface::StringType::MAX;
	*curSource = 0;
	disableAssertsEtc = 0;
	curFrameEntry = nullptr;
	firstPatternEntry = nullptr;
	lastPatternEntry = nullptr;
	*curCommandGroup = 0;
	alwaysFlush = false;
	timeStamp = false;
	m_radix = 10;
	m_fillChar = ' ';
	m_width = 0;
	m_isWindowed = 0;
	hrTranslators = nullptr;
	numHrTranslators = 0;
	firstIOFactory = nullptr;
	firstCmdGroup = nullptr;
	memset(frameHash, 0, sizeof(frameHash));
	nextUnusedFrameHash = nullptr;
	numAvailableFrameHash = 0;
	firstLogGroup = nullptr;
}

Debug::~Debug()
{
}

// Static initialization
void Debug::PreStaticInit(void)
{
	atexit(StaticExit);
}

void Debug::PostStaticInit(void)
{
}

void Debug::StaticExit(void)
{
}

// These must match the friend declarations in debug_debug.h
void __attribute__((constructor(101))) GccPreStaticInit() { Debug::PreStaticInit(); }
void __attribute__((constructor(65434))) GccPostStaticInit() { Debug::PostStaticInit(); }

void Debug::InstallExceptionHandler(void)
{
}

// Core debug functionality needed by DCRASH_RELEASE
bool Debug::SkipNext(void)
{
	return false;
}

Debug &Debug::CrashBegin(const char *file, int line)
{
	if (file)
		fprintf(stderr, "CRASH at %s:%d: ", file, line);
	else
		fprintf(stderr, "CRASH: ");
	return Instance;
}

bool Debug::CrashDone(bool die)
{
	fprintf(stderr, "\n");
	fflush(stderr);
	if (die)
		abort();
	return false;
}

Debug &Debug::AssertBegin(const char *file, int line, const char *expr)
{
	if (expr)
		fprintf(stderr, "ASSERT FAILED at %s:%d: %s ", file, line, expr);
	else
		fprintf(stderr, "ASSERT FAILED at %s:%d ", file, line);
	return Instance;
}

bool Debug::AssertDone(void)
{
	fprintf(stderr, "\n");
	fflush(stderr);
	return false;
}

Debug &Debug::CheckBegin(const char *file, int line, const char *expr)
{
	fprintf(stderr, "CHECK FAILED at %s:%d: %s ", file, line, expr);
	return Instance;
}

bool Debug::CheckDone(void)
{
	fprintf(stderr, "\n");
	fflush(stderr);
	return false;
}

Debug &Debug::LogBegin(const char *fileOrGroup)
{
	return Instance;
}

bool Debug::LogDone(void)
{
	return false;
}

bool Debug::IsLogEnabled(const char *)
{
	return false;
}

// Stream operators
Debug& Debug::operator<<(const char *str)
{
	if (str) fprintf(stderr, "%s", str);
	return *this;
}

Debug& Debug::operator<<(int val)
{
	fprintf(stderr, "%d", val);
	return *this;
}

Debug& Debug::operator<<(unsigned val)
{
	fprintf(stderr, "%u", val);
	return *this;
}

Debug& Debug::operator<<(long val)
{
	fprintf(stderr, "%ld", val);
	return *this;
}

Debug& Debug::operator<<(unsigned long val)
{
	fprintf(stderr, "%lu", val);
	return *this;
}

Debug& Debug::operator<<(bool val)
{
	fprintf(stderr, val ? "true" : "false");
	return *this;
}

Debug& Debug::operator<<(float val)
{
	fprintf(stderr, "%f", val);
	return *this;
}

Debug& Debug::operator<<(double val)
{
	fprintf(stderr, "%f", val);
	return *this;
}

Debug& Debug::operator<<(short val)
{
	fprintf(stderr, "%d", (int)val);
	return *this;
}

Debug& Debug::operator<<(unsigned short val)
{
	fprintf(stderr, "%u", (unsigned)val);
	return *this;
}

Debug& Debug::operator<<(__int64 val)
{
	fprintf(stderr, "%lld", (long long)val);
	return *this;
}

Debug& Debug::operator<<(unsigned __int64 val)
{
	fprintf(stderr, "%llu", (unsigned long long)val);
	return *this;
}

Debug& Debug::operator<<(const void *ptr)
{
	fprintf(stderr, "%p", ptr);
	return *this;
}

Debug& Debug::operator<<(const MemDump &)
{
	fprintf(stderr, "[memdump]");
	return *this;
}

Debug& Debug::operator<<(HResult hres)
{
	fprintf(stderr, "HRESULT(0x%08lx)", hres.m_hresult);
	return *this;
}

Debug& Debug::operator<<(RepeatChar c)
{
	for (int i = 0; i < c.m_count; ++i)
		fputc(c.m_char, stderr);
	return *this;
}

void Debug::SetPrefixAndRadix(const char *prefix, int radix)
{
	strncpy(m_prefix, prefix, sizeof(m_prefix) - 1);
	m_prefix[sizeof(m_prefix) - 1] = 0;
	m_radix = radix;
}

Debug::Format::Format(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(m_buffer, sizeof(m_buffer), format, args);
	va_end(args);
}

Debug::LogDescription::LogDescription(const char *, const char *)
{
}

// Registration functions
bool Debug::AddIOFactory(const char *, const char *, DebugIOInterface* (*)(void))
{
	return true;
}

bool Debug::AddCommands(const char *, DebugCmdInterface *)
{
	return true;
}

void Debug::RemoveCommands(DebugCmdInterface *)
{
}

void Debug::Command(const char *)
{
}

void Debug::Update(void)
{
}

void Debug::AddHResultTranslator(unsigned, HResultTranslator, void *)
{
}

void Debug::RemoveHResultTranslator(HResultTranslator, void *)
{
}

bool Debug::SimpleMatch(const char *str, const char *pattern)
{
	if (!str || !pattern) return false;
	while (*str && *pattern)
	{
		if (*pattern == '*')
		{
			pattern++;
			while (*str)
				if (SimpleMatch(str++, pattern))
					return true;
			return *str == *pattern;
		}
		else
		{
			if (*str++ != *pattern++)
				return false;
		}
	}
	return *str == *pattern;
}

void Debug::SetBuildInfo(const char *version, const char *internalVersion, const char *buildDate)
{
	if (version) strncpy(Instance.m_version, version, sizeof(Instance.m_version) - 1);
	if (internalVersion) strncpy(Instance.m_intVersion, internalVersion, sizeof(Instance.m_intVersion) - 1);
	if (buildDate) strncpy(Instance.m_buildDate, buildDate, sizeof(Instance.m_buildDate) - 1);
}

void Debug::WriteBuildInfo(void)
{
}

// Private helpers (stubs)
void Debug::StartOutput(DebugIOInterface::StringType, const char *, ...)
{
}

void Debug::AddOutput(const char *, unsigned)
{
}

void Debug::FlushOutput(bool)
{
}

Debug::FrameHashEntry *Debug::AddFrameEntry(unsigned, unsigned, const char *, int)
{
	return nullptr;
}

void Debug::UpdateFrameStatus(FrameHashEntry &)
{
}

void Debug::AddPatternEntry(unsigned, bool, const char *)
{
}

void Debug::ExecCommand(const char *, const char *)
{
}

bool Debug::IsWindowed(void)
{
	return true;
}

const char *Debug::AddLogGroup(const char *, const char *)
{
	return nullptr;
}

// Exception handler stubs
DebugExceptionhandler::DebugExceptionhandler(void) {}
DebugExceptionhandler::~DebugExceptionhandler() {}

// Stack walk stubs
DebugStackwalk::DebugStackwalk(void) {}
DebugStackwalk::~DebugStackwalk(void) {}

// Heap abort stub
void __cdecl _heap_abort(void)
{
	DCRASH_RELEASE("Fatal heap error.");
}

#endif // !_WIN32
