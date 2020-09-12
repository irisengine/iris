#include "networking/channel/reliable_ordered_channel.h"
#include <algorithm>
#include <vector>

namespace iris
{

ReliableOrderedChannel::ReliableOrderedChannel()
    : Channel()
    , next_receive_seq_(0u)
    , out_sequence_(0u)
{
}

void ReliableOrderedChannel::enqueue_send(Packet packet)
{
    // set sequence number of each packet to be once greater than the previous
    packet.set_sequence(out_sequence_);
    ++out_sequence_;

    send_queue_.emplace_back(std::move(packet));
}

void ReliableOrderedChannel::enqueue_receive(Packet packet)
{
    if (packet.type() == PacketType::ACK)
    {
        // we got an ack so remove the corresponding packet from the send queue
        // also use the opportunity to purge acks we've sent from the send queue
        send_queue_.erase(
            std::remove_if(
                std::begin(send_queue_),
                std::end(send_queue_),
                [&packet](const Packet &p) {
                    return (p.type() == PacketType::ACK) ||
                           (p.sequence() == packet.sequence());
                }),
            std::end(send_queue_));
    }
    else
    {
        // we got non-ack i.e. something we will want to yield

        // we only care about packets which are the one we are expecting or
        // after, anything before will have been yielded
        if (packet.sequence() >= next_receive_seq_)
        {
            // calculate index of packet into our receive queue
            const auto index = packet.sequence() - next_receive_seq_;

            // if index is larger than queue then grow the queue
            if (index >= receive_queue_.size())
            {
                receive_queue_.resize(receive_queue_.size() + index + 1u);
            }

            // if this is a new packet i.e. not a duplicate then put it in the
            // queue
            if (!receive_queue_[index].is_valid())
            {
                receive_queue_[index] = packet;
            }
        }

        // always send an ack, this is because acks aren't reliable so we may
        // keep receiving the same packet until an ack finally makes it
        // this is why we discard duplicates but still ack
        Packet ack{PacketType::ACK, ChannelType::RELIABLE_ORDERED, {}};
        ack.set_sequence(packet.sequence());
        send_queue_.emplace_back(std::move(ack));
    }
}

std::vector<Packet> ReliableOrderedChannel::yield_send_queue()
{
    // note that we don't yield the queue but return a copy, this is because
    // we want to keep sending packets until we receive an ack

    return send_queue_;
}

std::vector<Packet> ReliableOrderedChannel::yield_receive_queue()
{
    // find the first non-valid packet, everything before that will be a
    // continuous range of valid packets ready to be yielded
    const auto end_of_valid = std::find_if(
        std::cbegin(receive_queue_),
        std::cend(receive_queue_),
        [](const Packet &element) { return !element.is_valid(); });

    std::vector<Packet> packets{};

    if (end_of_valid != std::cbegin(receive_queue_))
    {
        // move packets from queue to output collection
        packets =
            std::vector<Packet>(std::cbegin(receive_queue_), end_of_valid);
        receive_queue_.erase(std::begin(receive_queue_), end_of_valid);

        // our next expected sequence number will be one greater than the last
        // packet we yield
        next_receive_seq_ = packets.back().sequence() + 1u;
    }

    return packets;
}

}
