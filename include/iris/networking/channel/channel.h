////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "networking/packet.h"
#include <deque>
#include <vector>

namespace iris
{

/**
 * Abstract class for a channel, a class which enforces guarantees on unreliable
 * packet transfer. This class provides an interface for consuming incoming
 * and outgoing packets, which when yielded will be in order according to
 * the channel guarantees.
 *
 * For example a channel which provides reliable and ordered guarantee will only
 * yield packets that are in order, it will buffer out of order packets until
 * the gaps have also been received. It will also handle acks so that dropped
 * packets will be resent.
 *
 * This is built on top of the Packet primitive.
 *
 * Note that this interface is not responsible for the actual sending and
 * receiving of packets. Rather it is a buffer which all packets being sent
 * and received should be passed through, anything yielded can then be sent
 * over a socket or passed up the application.
 *
 *        local channel                    remote channel
 *     +----------------+                +----------------+
 * --->|===== send =====|Y~~~~~~~~~~~~~~~|=== received ===|Y-->
 *     |                |                |                |
 * <--Y|=== received ===|~~~~~~~~~~~~~~~Y|===== send =====|<---
 *     +----------------+                +----------------+
 *
 * Y - yield
 */
class Channel
{
  public:
    Channel() = default;
    virtual ~Channel() = default;

    /**
     * Enqueue a packet to be sent.
     *
     * @param packet
     *   Packet to be sent.
     */
    virtual void enqueue_send(Packet packet) = 0;

    /**
     * Enqueue a received packet.
     *
     * @param packet
     *   Packet received.
     */
    virtual void enqueue_receive(Packet packet) = 0;

    /**
     * Yield all packets to be sent, according to the channel guarantees.
     *
     * @returns
     *   Packets to be send.
     */
    virtual std::vector<Packet> yield_send_queue();

    /**
     * Yield all packets that have been received, according to the channel
     * guarantees.
     *
     * @returns
     *   Packets received.
     */
    virtual std::vector<Packet> yield_receive_queue();

  protected:
    /** Queue for send packets. */
    std::vector<Packet> send_queue_;

    /** Queue for received packets. */
    std::vector<Packet> receive_queue_;
};

}
