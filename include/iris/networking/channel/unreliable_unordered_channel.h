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
