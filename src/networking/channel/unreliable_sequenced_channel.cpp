#include "networking/channel/unreliable_sequenced_channel.h"
#include <vector>

namespace eng
{

UnreliableSequencedChannel::UnreliableSequencedChannel()
    : Channel(),
      min_sequence_(0u),
      send_sequence_(0u)
{ }

void UnreliableSequencedChannel::enqueue_send(Packet packet)
{
    // set sequence number of each packet to be once greater than the previous
    packet.set_sequence(send_sequence_);
    send_queue_.emplace_back(std::move(packet));

    ++send_sequence_;
}

void UnreliableSequencedChannel::enqueue_receive(Packet packet)
{
    // discard all packets that are behind the largest sequence number we've
    // seen
    if(packet.sequence() >= min_sequence_)
    {
        receive_queue_.emplace_back(std::move(packet));

        // by always incrementing here we automatically drop duplicates
        min_sequence_ = receive_queue_.back().sequence() + 1u;
    }
}

}
