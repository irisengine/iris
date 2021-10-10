////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

// this file abstracts away platform specific networking includes as we as
// several utility functions
// it should suffice to just include this file to use BSD socket functions

#include <functional>

#if defined(IRIS_PLATFORM_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include "networking/win32/winsock.h"
static iris::Winsock ws_init;
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "core/exception.h"

namespace iris
{

#if defined(IRIS_PLATFORM_WIN32)

using SocketHandle = SOCKET;
static std::function<int(SocketHandle)> CloseSocket = ::closesocket;

/**
 * Set whether a socket should block on read.
 *
 * @param socket
 *   Handle to socket to change.
 *
 * @param blocking
 *   Whether socket should block or not.
 */
inline void set_blocking(SocketHandle socket, bool blocking)
{
    u_long mode = blocking ? 0 : 1;
    if (::ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR)
    {
        throw Exception("could not set blocking mode");
    }
}

/**
 * Check if the last read call would have blocked. i.e. no data was available.
 *
 * Note this is function is only valid following a read on a non-blocking
 * socket.
 *
 * @returns
 *   True if read would have blocked, false otherwise.
 */
inline bool last_call_blocked()
{
    return ::WSAGetLastError() == WSAEWOULDBLOCK;
}

#else

using SocketHandle = int;
static std::function<int(SocketHandle)> CloseSocket = ::close;
// this is already defined on windows so we define it again for consistency
#define INVALID_SOCKET -1

/**
 * Set whether a socket should block on read.
 *
 * @param socket
 *   Handle to socket to change.
 *
 * @param blocking
 *   Whether socket should block or not.
 */
inline void set_blocking(SocketHandle socket, bool blocking)
{
    auto flags = ::fcntl(socket, F_GETFL, 0);
    if (flags == -1)
    {
        throw Exception("could not get flags");
    }

    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

    if (::fcntl(socket, F_SETFL, flags) != 0)
    {
        throw Exception("could not set flags");
    }
}

/**
 * Check if the last read call would have blocked. i.e. no data was available.
 *
 * Note this is function is only valid following a read on a non-blocking
 * socket.
 *
 * @returns
 *   True if read would have blocked, false otherwise.
 */
inline bool last_call_blocked()
{
    return errno == EWOULDBLOCK;
}
#endif

}
