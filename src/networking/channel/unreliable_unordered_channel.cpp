#include "networking/channel/unreliable_unordered_channel.h"

namespace eng
{

void UnreliableUnorderedChannel::enqueue_send(Packet packet)
{
    send_queue_.emplace_back(std::move(packet));
}

void UnreliableUnorderedChannel::enqueue_receive(Packet packet)
{
    receive_queue_.emplace_back(std::move(packet));
}

}
