#include "networking/client_connection_handler.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <limits>
#include <memory>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/root.h"
#include "jobs/concurrent_queue.h"
#include "jobs/job_system.h"
#include "log/log.h"
#include "networking/channel/channel.h"
#include "networking/channel/reliable_ordered_channel.h"
#include "networking/channel/unreliable_sequenced_channel.h"
#include "networking/channel/unreliable_unordered_channel.h"
#include "networking/data_buffer_deserialiser.h"
#include "networking/data_buffer_serialiser.h"
#include "networking/packet.h"
#include "networking/posix/auto_socket.h"
#include "networking/socket.h"

namespace
{

/**
 * Initiate and perform a handshake with the server.
 *
 * @param socket
 *   Socket for the connection.
 *
 * @param channel
 *   Channel to perform handshake on.
 */
std::uint32_t handshake(iris::Socket *socket, iris::Channel *channel)
{
    auto id = std::numeric_limits<std::uint32_t>::max();

    // create and enqueue a HELLO packet
    static const auto hello = iris::Packet(
        iris::PacketType::HELLO, iris::ChannelType::RELIABLE_ORDERED, {});
    channel->enqueue_send(hello);

    // send all packets
    for (const auto &packet : channel->yield_send_queue())
    {
        socket->write(packet.data(), packet.packet_size());
    }

    // keep going until we complete handshake
    for (;;)
    {
        // read a packet
        const auto raw_packet = socket->read(sizeof(iris::Packet));
        iris::Packet packet{};
        std::memcpy(packet.data(), raw_packet.data(), raw_packet.size());
        packet.resize(raw_packet.size());

        // enqueue the packet into the channel
        channel->enqueue_receive(std::move(packet));

        // get all received packets
        const auto responses = channel->yield_receive_queue();

        // find the CONNECTED packet
        const auto connected = std::find_if(
            std::cbegin(responses),
            std::cend(responses),
            [](const iris::Packet &p) {
                return p.type() == iris::PacketType::CONNECTED;
            });

        // if we got it then get the id from the server and stop looping
        if (connected != std::cend(responses))
        {
            iris::DataBufferDeserialiser deserialiser{connected->body_buffer()};
            id = deserialiser.pop<std::uint32_t>();
            break;
        }
    }

    if (id == std::numeric_limits<std::uint32_t>::max())
    {
        throw iris::Exception("connection timeout");
    }

    LOG_ENGINE_INFO("client_connection_handler", "i am: {}", id);

    return id;
}

/**
 * Helper function to handle the start of a sync.
 *
 * @param channel
 *   The channel to communicate on.
 *
 * @param socket
 *   Socket for the connection.
 */
void handle_sync_start(iris::Channel *channel, iris::Socket *socket)
{
    // serialise our time
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
    iris::DataBufferSerialiser serialiser{};
    serialiser.push<std::uint32_t>(now.count());

    // create and enqueue SYNC_RESPONSE
    iris::Packet response{
        iris::PacketType::SYNC_RESPONSE,
        iris::ChannelType::RELIABLE_ORDERED,
        serialiser.data()};
    channel->enqueue_send(std::move(response));

    // send all packets
    for (const auto &packet : channel->yield_send_queue())
    {
        socket->write(packet.data(), packet.packet_size());
    }
}

/**
 * Helper function to handle sync finish.
 *
 * @param packet
 *   SYNC_FINSIH packet.
 *
 * @returns
 *   Estimate of lag between client and server.
 */
std::chrono::milliseconds handle_sync_finish(const iris::Packet &packet)
{
    // deserialise times sent from server
    iris::DataBufferDeserialiser deserialiser{packet.body_buffer()};
    const auto [client_time_raw, server_time_raw] =
        deserialiser.pop_tuple<std::uint32_t, std::uint32_t>();
    const std::chrono::milliseconds client_time(client_time_raw);
    const std::chrono::milliseconds server_time(server_time_raw);

    // estimate lag
    const auto server_to_client =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch() - server_time);
    const auto client_to_server = server_time - client_time;

    return server_to_client + client_to_server;
}

}

namespace iris
{

ClientConnectionHandler::ClientConnectionHandler(std::unique_ptr<Socket> socket)
    : socket_(std::move(socket))
    , id_(std::numeric_limits<std::uint32_t>::max())
    , lag_(0u)
    , channels_()
    , queues_()
{
    // setup channels
    channels_[ChannelType::UNRELIABLE_UNORDERED] =
        std::make_unique<UnreliableUnorderedChannel>();
    channels_[ChannelType::UNRELIABLE_SEQUENCED] =
        std::make_unique<UnreliableSequencedChannel>();
    channels_[ChannelType::RELIABLE_ORDERED] =
        std::make_unique<ReliableOrderedChannel>();

    queues_[ChannelType::UNRELIABLE_UNORDERED] =
        std::make_unique<ConcurrentQueue<DataBuffer>>();
    queues_[ChannelType::UNRELIABLE_SEQUENCED] =
        std::make_unique<ConcurrentQueue<DataBuffer>>();
    queues_[ChannelType::RELIABLE_ORDERED] =
        std::make_unique<ConcurrentQueue<DataBuffer>>();

    id_ = handshake(
        socket_.get(), channels_[ChannelType::RELIABLE_ORDERED].get());

    LOG_ENGINE_INFO("client_connection_handler", "connected!");

    // we want to continually read data as fast as possible, so we do reading in
    // a background job
    // this will handle any protocol packets and stick data into queues, which
    // can then be retrieved by calls to try_read
    iris::Root::job_system().add_jobs({[this]() {
        for (;;)
        {
            // block and read the next Packet
            const auto raw_packet = socket_->read(sizeof(Packet));
            iris::Packet packet{};
            std::memcpy(packet.data(), raw_packet.data(), raw_packet.size());
            packet.resize(raw_packet.size());

            // enqueue the packet into the right channel
            const auto channel_type = packet.channel();
            auto *channel = channels_.at(channel_type).get();
            channel->enqueue_receive(std::move(packet));

            // handle all received packets from that channel
            for (const auto &p : channel->yield_receive_queue())
            {
                switch (p.type())
                {
                    case PacketType::DATA:
                        // we got data, stick it in the queue for this channel
                        queues_[channel_type]->enqueue(p.body_buffer());
                        break;
                    case PacketType::SYNC_START:
                        handle_sync_start(channel, socket_.get());
                        break;
                    case PacketType::SYNC_FINISH:
                        lag_ = handle_sync_finish(packet);
                        break;
                    default:
                        LOG_ERROR(
                            "client_connection_handler",
                            "unknown packet type {}",
                            (int)p.type());
                        break;
                }
            }
        }
    }});
}

std::optional<DataBuffer> ClientConnectionHandler::try_read(
    ChannelType channel_type)
{
    DataBuffer buffer;

    // try and read data from the supplied channel
    return queues_[channel_type]->try_dequeue(buffer)
               ? std::optional<DataBuffer>{buffer}
               : std::nullopt;
}

void ClientConnectionHandler::send(
    const DataBuffer &data,
    ChannelType channel_type)
{
    auto *channel = channels_[channel_type].get();

    // wrap data in a Packet and enqueue
    Packet packet{PacketType::DATA, channel_type, data};
    channel->enqueue_send(std::move(packet));

    // send all packets
    for (const auto &p : channel->yield_send_queue())
    {
        socket_->write(p.data(), p.packet_size());
    }
}

std::uint32_t ClientConnectionHandler::id() const
{
    return id_;
}

std::chrono::milliseconds ClientConnectionHandler::lag() const
{
    return lag_;
}

}
