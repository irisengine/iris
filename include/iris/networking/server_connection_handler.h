////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "core/context.h"
#include "core/data_buffer.h"
#include "networking/channel/channel.h"
#include "networking/channel/channel_type.h"
#include "networking/server_socket.h"

namespace iris
{

/**
 * This class provides an abstraction over the low-level handling of
 * connections. It automatically handles:
 *  - accepting connections
 *  - handshakes
 *  - clock syncs
 *  - sending/receiving data
 *
 * This is all done with the lightweight Packet protocol and Channels. This
 * class can also be provided with callbacks for key events.
 *
 * Protocol:
 *
 * Handshake - must be performed when client connections.
 *
 * client           server
 *          HELLO
 *        -------->
 *
 *         CONNECTED
 *           [id]
 *        <--------
 *
 * Data - this is sent via DATA packets, ACKs may be sent in response depending
 * on the channel used.
 *
 * Sync - this allows the client to synchronise its clock with the server,
 * always happens after handshake but my happen again if the server thinks the
 * client is out of sync.
 *
 * client           server
 *        SYNC_START
 *        <--------
 *
 *      SYNC_RESPONSE
 *      [client_time]
 *        -------->
 *
 *       SYNC_START
 *      [client_time]
 *      [server_time]
 *        <--------
 */
class ServerConnectionHandler
{
  public:
    /**
     * Callback for new connections
     *
     * @param id
     *   A unique id for this connection.
     */
    using NewConnectionCallback = std::function<void(std::size_t id)>;

    /**
     * Callback for when a connection sends data.
     *
     * @param id
     *   Id of connection sending data.
     *
     * @param data
     *   The data send.
     *
     * @param channel
     *   The channel type the client sent the data on.
     */
    using RecvCallback = std::function<void(std::size_t id, const DataBuffer &data, ChannelType channel)>;

    /**
     * Create a new ServerConnectionHandler.
     *
     * @param context
     *   Engine context object.

     * @param socket
     *   The underlying socket to use.
     *
     * @param new_connection
     *   Callback to fire when a new connection is created.
     *
     * @param recv
     *   Callback to fire when data is received.
     */
    ServerConnectionHandler(
        Context &context,
        std::unique_ptr<ServerSocket> socket,
        NewConnectionCallback new_connection,
        RecvCallback recv);

    // defined in implementation
    ~ServerConnectionHandler();

    // deleted
    ServerConnectionHandler(const ServerConnectionHandler &) = delete;
    ServerConnectionHandler &operator=(const ServerConnectionHandler &) = delete;

    /**
     * Updates the connection handler, processes all messages and fires all
     * callbacks. This should be called regularly (e.g. from a game loop)
     */
    void update();

    /**
     * Send data to a connection.
     *
     * @param id
     *   Id of connection to send data to.
     *
     * @param message
     *   Data to send.
     *
     * @param channel_type
     *   The channel to send the data through
     */
    void send(std::size_t id, const DataBuffer &message, ChannelType channel_type);

  private:
    // forward declare internal struct
    struct Connection;

    /** Underlying socket. */
    std::unique_ptr<ServerSocket> socket_;

    /** New connection callback. */
    NewConnectionCallback new_connection_callback_;

    /** Received data callback. */
    RecvCallback recv_callback_;

    /** Start time of connection handler. */
    std::chrono::steady_clock::time_point start_;

    /** Map of connections to their unique id. */
    std::map<std::size_t, std::unique_ptr<Connection>> connections_;

    /** Mutex to control access to messages. */
    std::mutex mutex_;

    /** Collection of messages. */
    std::vector<DataBuffer> messages_;
};

}
