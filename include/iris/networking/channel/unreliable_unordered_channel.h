////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "networking/channel/channel.h"

namespace iris
{

/**
 * Implementation of Channel with no guarantees. This is the most basic channel
 * and is effectively a no-op for unreliable traffic.
 */
class UnreliableUnorderedChannel : public Channel
{
  public:
    // default
    ~UnreliableUnorderedChannel() override = default;

    /**
     * Enqueue a packet to be sent.
     *
     * @param packet
     *   Packet to be sent.
     */
    void enqueue_send(Packet packet) override;

    /**
     * Enqueue a received packet.
     *
     * @param packet
     *   Packet received.
     */
    void enqueue_receive(Packet packet) override;
};

}
