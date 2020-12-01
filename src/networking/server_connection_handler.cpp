#include "networking/server_connection_handler.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <vector>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/root.h"
#include "jobs/concurrent_queue.h"
#include "jobs/job_system.h"
#include "log/log.h"
#include "networking/channel/channel_type.h"
#include "networking/channel/reliable_ordered_channel.h"
#include "networking/channel/unreliable_sequenced_channel.h"
#include "networking/channel/unreliable_unordered_channel.h"
#include "networking/data_buffer_deserialiser.h"
#include "networking/data_buffer_serialiser.h"
#include "networking/packet.h"
#include "networking/socket.h"

namespace
{

/**
 * Helper function to handle a hello message. This is the first part of the
 * handshake and the server needs to respond with CONNECTED. We also use this
 * opportunity to start a sync request.
 *
 * @param id
 *   Id of connection.
 *
 * @param channel
 *   The channel HELLO was received on.
 *
 * @param socket
 *   Socket for the connection.
 */
void handle_hello(std::size_t id, iris::Channel *channel, iris::Socket *socket)
{
    // we will send the client their id
    iris::DataBufferSerialiser serialiser{};
    serialiser.push<std::uint32_t>(id);

    // create and enqueue response packets
    iris::Packet connected{
        iris::PacketType::CONNECTED,
        iris::ChannelType::RELIABLE_ORDERED,
        serialiser.data()};
    iris::Packet sync_start{
        iris::PacketType::SYNC_START, iris::ChannelType::RELIABLE_ORDERED, {}};
    channel->enqueue_send(std::move(connected));
    channel->enqueue_send(std::move(sync_start));

    // send all packets
    for (const auto &packet : channel->yield_send_queue())
    {
        socket->write(packet.data(), packet.packet_size());
    }
}

/**
 * Helper function to handle the response to a sync.
 *
 * @param channel
 *   The channel to communicate on.
 *
 * @param socket
 *   Socket for the connection.
 *
 * @param packet
 *   The received SYNC_RESPONSE packet.
 */
void handle_sync_response(
    iris::Channel *channel,
    iris::Socket *socket,
    const iris::Packet &packet)
{
    // get the client time and our time
    iris::DataBufferDeserialiser deserialiser{packet.body_buffer()};
    const auto client_time_raw = deserialiser.pop<std::uint32_t>();
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());

    // send the client back their time and out time
    iris::DataBufferSerialiser serialiser{};
    serialiser.push(client_time_raw);
    serialiser.push<std::uint32_t>(now.count());
    iris::Packet sync_finish{
        iris::PacketType::SYNC_FINISH,
        iris::ChannelType::RELIABLE_ORDERED,
        serialiser.data()};
    channel->enqueue_send(std::move(sync_finish));

    // send all packets
    for (const auto &p : channel->yield_send_queue())
    {
        socket->write(p.data(), p.packet_size());
    }
}

}

namespace iris
{

/**
 * Helper struct to encapsulate data for a connection.
 */
struct ServerConnectionHandler::Connection
{
    Socket *socket;
    std::map<ChannelType, std::unique_ptr<Channel>> channels;
    std::chrono::milliseconds rtt;
};

ServerConnectionHandler::ServerConnectionHandler(
    std::unique_ptr<AcceptingSocket> socket,
    NewConnectionCallback new_connection_callback,
    RecvCallback recv_callback)
    : socket_(std::move(socket))
    , new_connection_callback_(new_connection_callback)
    , recv_callback_(recv_callback)
    , start_(std::chrono::steady_clock::now())
    , connections()
    , mutex()
    , messages()
{
    // we want to always be accepting connections, so we do this in a background
    // job
    Root::job_system().add_jobs({[this]() {
        static std::size_t id = 0u;

        for (;;)
        {
            auto *socket = socket_->accept();

            LOG_ENGINE_INFO("server_connection_handler", "accepted connection");

            if (socket != nullptr)
            {
                // setup internal struct to manage connection
                auto connection = std::make_unique<Connection>();
                connection->socket = socket;
                connection->channels[ChannelType::UNRELIABLE_UNORDERED] =
                    std::make_unique<UnreliableUnorderedChannel>();
                connection->channels[ChannelType::UNRELIABLE_SEQUENCED] =
                    std::make_unique<UnreliableSequencedChannel>();
                connection->channels[ChannelType::RELIABLE_ORDERED] =
                    std::make_unique<ReliableOrderedChannel>();

                std::unique_lock lock(mutex);

                connections[id] = std::move(connection);
                ++id;
            }
        }
    }});
}

ServerConnectionHandler::~ServerConnectionHandler() = default;

void ServerConnectionHandler::update()
{
    std::unique_lock lock(mutex);

    // process each connection
    for (auto &[id, connection] : connections)
    {
        // has the connection sent us any data?
        const auto raw_packet = connection->socket->try_read(1024);
        if (raw_packet)
        {
            // convert data into a Packet
            iris::Packet packet{};
            std::memcpy(packet.data(), raw_packet->data(), raw_packet->size());
            packet.resize(raw_packet->size());

            // enqueue the packet into the right channel
            const auto channel_type = packet.channel();
            auto *channel = connection->channels.at(channel_type).get();
            channel->enqueue_receive(std::move(packet));

            // handle all received packets from that channel
            for (const auto &p : channel->yield_receive_queue())
            {
                switch (p.type())
                {
                    case PacketType::HELLO:
                    {
                        handle_hello(id, channel, connection->socket);

                        // we got a new client, fire it back to the application
                        new_connection_callback_(id);
                        break;
                    }
                    case PacketType::DATA:
                    {
                        // we got data, fire it back to the application
                        recv_callback_(id, p.body_buffer(), p.channel());
                        break;
                    }
                    case PacketType::SYNC_RESPONSE:
                    {
                        handle_sync_response(
                            channel, connection->socket, packet);
                        break;
                    }
                    default:
                        LOG_ENGINE_ERROR(
                            "server_connection_handler", "unknown packet type");
                }
            }
        }
    }
}

void ServerConnectionHandler::send(
    std::size_t id,
    const DataBuffer &message,
    ChannelType channel_type)
{
    Channel *channel = nullptr;
    Socket *socket = nullptr;

    {
        std::unique_lock lock(mutex);
        channel = connections[id]->channels[channel_type].get();
        socket = connections[id]->socket;
    }

    // wrap data in a Packet and enqueue
    Packet packet(PacketType::DATA, channel_type, message);
    channel->enqueue_send(std::move(packet));

    // send all packets
    for (const auto &p : channel->yield_send_queue())
    {
        socket->write(p.data(), p.packet_size());
    }
}

}
