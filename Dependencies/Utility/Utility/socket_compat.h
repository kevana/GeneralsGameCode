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

// This file contains socket related functions for compatibility with non-windows platforms.
#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// SOCKET type
typedef int SOCKET;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

// closesocket -> close on POSIX
#define closesocket close

// WSAStartup/WSACleanup are no-ops on POSIX
struct WSAData { int wVersion; int wHighVersion; };
typedef WSAData WSADATA;

inline int WSAStartup(unsigned short wVersionRequested, WSADATA* lpWSAData)
{
    if (lpWSAData) {
        lpWSAData->wVersion = wVersionRequested;
        lpWSAData->wHighVersion = wVersionRequested;
    }
    return 0;
}

inline int WSACleanup() { return 0; }

// WSAGetLastError -> errno on POSIX
inline int WSAGetLastError() { return errno; }

// MAKEWORD/LOBYTE/HIBYTE macros
#ifndef MAKEWORD
#define MAKEWORD(a, b) ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))
#endif
#ifndef LOBYTE
#define LOBYTE(w) ((unsigned char)(w))
#endif
#ifndef HIBYTE
#define HIBYTE(w) ((unsigned char)(((unsigned short)(w) >> 8) & 0xFF))
#endif

// ioctlsocket compatibility - FIONBIO for non-blocking mode
#ifndef FIONBIO
#include <sys/ioctl.h>
#endif

inline int ioctlsocket(int fd, long cmd, unsigned long* argp)
{
    if (cmd == FIONBIO) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) return SOCKET_ERROR;
        if (*argp)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        return fcntl(fd, F_SETFL, flags) < 0 ? SOCKET_ERROR : 0;
    }
    return SOCKET_ERROR;
}

// HOSTENT — Windows typedef alias for struct hostent
#ifndef HOSTENT
typedef struct hostent HOSTENT;
#endif

// WSA error code compatibility - map to POSIX errno values
#define WSAEINVAL         EINVAL
#define WSAEWOULDBLOCK    EWOULDBLOCK
#define WSAEINPROGRESS    EINPROGRESS
#define WSAEALREADY       EALREADY
#define WSAENOTSOCK       ENOTSOCK
#define WSAEDESTADDRREQ   EDESTADDRREQ
#define WSAEMSGSIZE       EMSGSIZE
#define WSAEPROTOTYPE     EPROTOTYPE
#define WSAENOPROTOOPT    ENOPROTOOPT
#define WSAEPROTONOSUPPORT EPROTONOSUPPORT
#define WSAEOPNOTSUPP     EOPNOTSUPP
#define WSAEAFNOSUPPORT   EAFNOSUPPORT
#define WSAEADDRINUSE     EADDRINUSE
#define WSAEADDRNOTAVAIL  EADDRNOTAVAIL
#define WSAENETDOWN       ENETDOWN
#define WSAENETUNREACH    ENETUNREACH
#define WSAENETRESET      ENETRESET
#define WSAECONNABORTED   ECONNABORTED
#define WSAECONNRESET     ECONNRESET
#define WSAENOBUFS        ENOBUFS
#define WSAEISCONN        EISCONN
#define WSAENOTCONN       ENOTCONN
#define WSAETIMEDOUT      ETIMEDOUT
#define WSAECONNREFUSED   ECONNREFUSED
#define WSAEHOSTDOWN      EHOSTDOWN
#define WSAEHOSTUNREACH   EHOSTUNREACH
