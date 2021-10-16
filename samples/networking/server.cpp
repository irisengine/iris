////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <thread>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/looper.h"
#include "core/vector3.h"
#include "log/log.h"
#include "networking/data_buffer_deserialiser.h"
#include "networking/data_buffer_serialiser.h"
#include "networking/networking.h"
#include "networking/packet.h"
#include "networking/server_connection_handler.h"
#include "networking/udp_server_socket.h"
#include "physics/basic_character_controller.h"
#include "physics/box_collision_shape.h"
#include "physics/physics_system.h"
#include "physics/rigid_body.h"
#include "events/keyboard_event.h"

#include "client_input.h"

using namespace std::chrono_literals;

iris::CharacterController *character_controller = nullptr;
std::size_t player_id = std::numeric_limits<std::size_t>::max();

void go(int, char **)
{
    iris::Logger::instance().set_log_engine(true);

    LOG_DEBUG("server_sample", "hello world");

    std::deque<ClientInput> inputs;
    auto tick = 0u;

    auto socket = std::make_unique<iris::UdpServerSocket>("127.0.0.1", 8888);

    iris::ServerConnectionHandler connection_handler(
        std::move(socket),
        [](std::size_t id) {
            LOG_DEBUG("server", "new connection {}", id);

            // just support a single player
            player_id = id;
        },
        [&inputs, &tick](
            std::size_t id,
            const iris::DataBuffer &data,
            iris::ChannelType type) {
            if (type == iris::ChannelType::RELIABLE_ORDERED)
            {
                iris::DataBufferDeserialiser deserialiser(data);
                ClientInput input{deserialiser};

                if (input.tick >= tick)
                {
                    // if input is for now or the future (which it should be as
                    // the client runs ahead) then store it
                    inputs.emplace_back(input);
                }
                else
                {
                    LOG_WARN("server", "stale input: {} {}", tick, input.tick);
                }
            }
        });

    iris::PhysicsSystem ps{};
    character_controller =
        ps.create_character_controller<iris::BasicCharacterController>(&ps);
    ps.create_rigid_body(
        iris::Vector3{0.0f, -50.0f, 0.0f},
        std::make_unique<iris::BoxCollisionShape>(
            iris::Vector3{500.0f, 50.0f, 500.0f}),
        iris::RigidBodyType::STATIC);
    auto *box = ps.create_rigid_body(
        iris::Vector3{0.0f, 1.0f, 0.0f},
        std::make_unique<iris::BoxCollisionShape>(
            iris::Vector3{0.5f, 0.5f, 0.5f}),
        iris::RigidBodyType::NORMAL);

    // block and wait for client to connect
    while (player_id == std::numeric_limits<std::size_t>::max())
    {
        connection_handler.update();
        std::this_thread::sleep_for(100ms);
    }

    std::chrono::microseconds step(0);

    ps.step(33ms);

    iris::Looper looper{
        0ms,
        33ms,
        [&](std::chrono::microseconds clock,
            std::chrono::microseconds time_step) {
            // fixed timestep function
            // this runs the physics and processes player input

            for (const auto &input : inputs)
            {
                // if stored input is for our current tick then apply it to
                // the physics simulation
                if (input.tick == tick)
                {
                    iris::Vector3 walk_direction{
                        input.side, 0.0f, input.forward};
                    walk_direction.normalise();

                    character_controller->set_walk_direction(walk_direction);
                }
                if (input.tick > tick)
                {
                    break;
                }
            }

            ps.step(33ms);
            ++tick;

            return true;
        },
        [&](std::chrono::microseconds clock,
            std::chrono::microseconds time_step) {
            // variable timestep function
            // sends snapshots of the world to the client

            connection_handler.update();

            // whilst this is a variable time function we only want to send out
            // updates every 100ms
            if (clock > step + 100ms)
            {
                // serialise world state
                iris::DataBufferSerialiser serialiser;
                serialiser.push(character_controller->position());
                serialiser.push(character_controller->linear_velocity());
                serialiser.push(character_controller->angular_velocity());
                serialiser.push(tick);
                serialiser.push(box->position());
                serialiser.push(box->orientation());

                connection_handler.send(
                    player_id,
                    serialiser.data(),
                    iris::ChannelType::RELIABLE_ORDERED);

                step = clock;
            }

            return true;
        }};

    looper.run();
}

int main(int argc, char **argv)
{
    go(argc, argv);

    return 0;
}
