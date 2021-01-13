#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <ratio>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/looper.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_entity.h"
#include "graphics/scene.h"
#include "graphics/stage.h"
#include "log/emoji_formatter.h"
#include "log/log.h"
#include "networking/client_connection_handler.h"
#include "networking/data_buffer_deserialiser.h"
#include "networking/data_buffer_serialiser.h"
#include "networking/packet.h"
#include "networking/udp_socket.h"
#include "physics/basic_character_controller.h"
#include "physics/box_collision_shape.h"
#include "physics/physics_system.h"
#include "physics/rigid_body.h"
#include "platform/keyboard_event.h"
#include "platform/start.h"
#include "platform/window.h"

#include "client_input.h"

using namespace std::chrono_literals;

/**
 * Process all pending user input. This will send input to the server as well as
 * store it locally.
 *
 * @param inputs
 *   Collection to store inputs in.
 *
 * @param camera
 *   Camera to update.
 *
 * @param tick
 *   Current client tick number.
 *
 * @param client
 *   Object to communicate with server.
 *
 * @returns
 *   True if user has quit, false otherwise.
 */
bool handle_input(
    std::deque<ClientInput> &inputs,
    iris::Camera &camera,
    std::uint32_t tick,
    iris::ClientConnectionHandler &client,
    iris::Window &window)
{
    auto quit = false;

    // we just continuously update a static input object, this ensures the
    // object always represents the current input state
    static ClientInput input;

    auto has_input = false;

    // consume all inputs
    for (;;)
    {
        auto evt = window.pump_event();
        if (!evt)
        {
            break;
        }

        if (evt->is_key(iris::Key::ESCAPE))
        {
            quit = true;
        }
        else if (evt->is_key())
        {
            const auto keyboard = evt->key();

            // convert input keys to client input
            switch (keyboard.key)
            {
                case iris::Key::W:
                    input.forward =
                        keyboard.state == iris::KeyState::DOWN ? -1.0f : 0.0f;
                    break;
                case iris::Key::S:
                    input.forward =
                        keyboard.state == iris::KeyState::DOWN ? 1.0f : 0.0f;
                    break;
                case iris::Key::A:
                    input.side =
                        keyboard.state == iris::KeyState::DOWN ? -1.0f : 0.0f;
                    break;
                case iris::Key::D:
                    input.side =
                        keyboard.state == iris::KeyState::DOWN ? 1.0f : 0.0f;
                    break;
                default: break;
            }

            has_input = true;
        }
        else if (evt->is_mouse())
        {
            // update camera based on mouse movement
            static const auto sensitivity = 0.0025f;
            const auto mouse = evt->mouse();

            camera.adjust_yaw(mouse.delta_x * sensitivity);
            camera.adjust_pitch(-mouse.delta_y * sensitivity);
        }
    }

    // if we processed any input then send the latest input state to the server
    // and store a copy locally
    if (has_input)
    {
        input.tick = tick;

        iris::DataBufferSerialiser serialiser;
        input.serialise(serialiser);
        client.send(serialiser.data(), iris::ChannelType::RELIABLE_ORDERED);

        inputs.emplace_back(input);
    }

    return quit;
}

/**
 * Process an update from the server on the state of the world.
 *
 * @param server_data
 *   Data from server
 *
 * @param snapshots
 *   Collection snapshots of server updates (of non player entity).
 *
 * @param history
 *   Collection of local state history.
 *
 * @param inputs
 *   Collection of stored user inputs.
 *
 * @returns
 *   Tuple of various server data.
 */
std::tuple<std::uint32_t, iris::Vector3, iris::Vector3, iris::Vector3>
process_server_update(
    const iris::DataBuffer &server_data,
    std::deque<std::tuple<
        std::chrono::steady_clock::time_point,
        iris::Vector3,
        iris::Quaternion>> &snapshots,
    std::vector<std::tuple<
        std::uint32_t,
        iris::Vector3,
        std::unique_ptr<iris::PhysicsState, iris::PhysicsStateDeleter>>>
        &history,
    std::deque<ClientInput> &inputs)
{
    // deserialise server update
    iris::DataBufferDeserialiser deserialiser(server_data);
    const auto position = deserialiser.pop<iris::Vector3>();
    const auto linear_velocity = deserialiser.pop<iris::Vector3>();
    const auto angular_velocity = deserialiser.pop<iris::Vector3>();
    const auto last_acked = deserialiser.pop<std::uint32_t>();
    const auto box_pos = deserialiser.pop<iris::Vector3>();
    auto box_rot = deserialiser.pop<iris::Quaternion>();
    box_rot.normalise();

    // store server update of box, put the time in the future so we can easily
    // interpolate between snapshots
    snapshots.emplace_back(
        std::chrono::steady_clock::now() + 100ms, box_pos, box_rot);

    // find the last input acknowledged by the server
    const auto acked_input = std::find_if(
        std::cbegin(inputs),
        std::cend(inputs),
        [last_acked](const ClientInput &element) {
            return element.tick >= last_acked;
        });

    // cleanup old inputs
    if (acked_input != std::cend(inputs))
    {
        inputs.erase(std::cbegin(inputs), acked_input);
    }

    // find last history entry acknowledged by the server
    const auto acked_history = std::find_if(
        std::cbegin(history),
        std::cend(history),
        [last_acked](const auto &element) {
            return std::get<0>(element) == last_acked;
        });

    // cleanup old history
    if (acked_history != std::cend(history))
    {
        history.erase(std::cbegin(history), acked_history);
    }

    // return useful data
    return {last_acked, position, linear_velocity, angular_velocity};
}

/**
 * Update the client prediction based on server updates. This will compare the
 * actual server position at a given tick to our historical prediction at that
 * time. If they differ by more than some threshold we will:
 *  - reset to that state
 *  - apply the correct server details
 *  - replay all user inputs since that tick
 *
 * @param last_acked
 *   The last tick the server acknowledged.
 *
 * @param history
 *   Collection of local state history.
 *
 * @param server_position
 *   The true position of the client at last_acked.
 *
 * @param linear_velocity
 *   The true linear_velocity of the client at last_acked.
 *
 * @param angular_velocity
 *   The true angular_velocity of the client at last_acked.
 *
 * @param character_controller
 *   Pointer to character controller.
 *
 * @param inputs
 *   Collection of stored user inputs.
 *
 * @param physics_system
 *   Physics system.
 */
void client_prediciton(
    std::uint32_t last_acked,
    std::vector<std::tuple<
        std::uint32_t,
        iris::Vector3,
        std::unique_ptr<iris::PhysicsState, iris::PhysicsStateDeleter>>>
        &history,
    const iris::Vector3 &server_position,
    const iris::Vector3 &linear_velocity,
    const iris::Vector3 &angular_velocity,
    iris::CharacterController *character_controller,
    std::deque<ClientInput> &inputs,
    iris::PhysicsSystem *physics_system)
{
    const auto &[tck_num, pos, state] = history.front();

    // as we periodically purge acked history if we have an entry for the last
    // acked tick it will be the first in our collection
    if (tck_num == last_acked)
    {
        // get the difference between our saved predicted position and the
        // actual server prediction
        static const auto threshold = 0.3f;
        const auto diff = std::abs((pos - server_position).magnitude());

        // if the difference is above our threshold (which account for floating
        // point rounding errors) then we have gone out of sync with the server
        if (diff >= threshold)
        {
            // reset to the stored state
            physics_system->load(state.get());

            // update the player with the server supplied data
            character_controller->reposition(
                server_position, iris::Quaternion{0.0f, 1.0f, 0.0f, 0.0f});
            character_controller->set_linear_velocity(linear_velocity);
            character_controller->set_angular_velocity(angular_velocity);

            // update the history entry
            history[0] = std::make_tuple(
                tck_num,
                character_controller->position(),
                physics_system->save());

            // update every other history entry by replaying user inputs
            for (auto i = 1u; i < history.size(); ++i)
            {
                auto current_tick = tck_num + i;

                // replay user inputs for our current history entry
                for (const auto &input : inputs)
                {
                    if (input.tick == current_tick)
                    {
                        iris::Vector3 walk_direction{
                            input.side, 0.0f, input.forward};
                        walk_direction.normalise();
                        character_controller->set_walk_direction(
                            walk_direction);
                    }
                }

                // step physics
                physics_system->step(std::chrono::milliseconds(33));

                // update history
                history[i] = std::make_tuple(
                    current_tick,
                    character_controller->position(),
                    physics_system->save());
            }
        }
    }

    // we should now be back in sync with the server
}

/**
 * Interpolate updates from the server. As the server send updates at fixed
 * intervals rendering each update would give jerky motion. Instead we delay
 * rendering by a small amount so we have two snapshots of the entity positions,
 * we can then interpolate between them for smoother motion.
 *
 * @param snapshots
 *   Collection snapshots of server updates (of non player entity).
 *
 * @param box
 *   Pointer to RenderEntity for box.
 */
void entity_interpolation(
    std::deque<std::tuple<
        std::chrono::steady_clock::time_point,
        iris::Vector3,
        iris::Quaternion>> &snapshots,
    iris::RenderEntity *box)
{
    const auto now = std::chrono::steady_clock::now();

    // find the first snapshot that is ahead of us in time, this will be the
    // snapshot we interpolate towards
    const auto second_snapshot = std::find_if(
        std::cbegin(snapshots) + 1u,
        std::cend(snapshots),
        [&now](const auto &element) { return std::get<0>(element) >= now; });

    // check we have two snapshots
    if (second_snapshot != std::cend(snapshots))
    {
        const auto first_snapshot = second_snapshot - 1u;

        auto [time_start, pos_start, rot_start] = *first_snapshot;
        const auto &[time_end, pos_end, rot_end] = *second_snapshot;

        // calculate the interpolate amount
        const auto delta1 = now - time_start;
        const auto delta2 = time_end - time_start;
        const auto lerp_amount = static_cast<float>(delta1.count()) /
                                 static_cast<float>(delta2.count());

        pos_start.lerp(pos_end, lerp_amount);
        rot_start.slerp(rot_end, lerp_amount);

        box->set_position(pos_start);
        box->set_orientation(rot_start);

        // purge old snapshots
        snapshots.erase(std::cbegin(snapshots), first_snapshot);
    }
}

void go(int, char **)
{
    LOG_DEBUG("client", "hello world");

    auto socket = std::make_unique<iris::UdpSocket>("127.0.0.1", 8888);

    iris::ClientConnectionHandler client{std::move(socket)};

    iris::Window window{800.0f, 800.0f};
    iris::Camera camera{iris::CameraType::PERSPECTIVE, 800.0f, 800.0f};

    auto scene = std::make_unique<iris::Scene>();

    scene->create_entity(
        iris::RenderGraph{},
        iris::mesh_factory::cube({1.0f}),
        iris::Vector3{0.0f, -50.0f, 0.0f},
        iris::Vector3{500.0f, 50.0f, 500.0f});

    auto *box = scene->create_entity(
        iris::RenderGraph{},
        iris::mesh_factory::cube({1.0f, 0.0f, 0.0f}),
        iris::Vector3{0.0f, 1.0f, 0.0f},
        iris::Vector3{0.5f, 0.5f, 0.5f});

    iris::Pipeline pipeline{};
    pipeline.add_stage(std::move(scene), camera);

    std::deque<std::tuple<
        std::chrono::steady_clock::time_point,
        iris::Vector3,
        iris::Quaternion>>
        snapshots;
    std::deque<ClientInput> inputs;

    iris::PhysicsSystem ps{};
    auto *character_controller =
        ps.create_character_controller<iris::BasicCharacterController>(&ps);
    ps.create_rigid_body(
        iris::Vector3{0.0f, -50.0f, 0.0f},
        std::make_unique<iris::BoxCollisionShape>(
            iris::Vector3{500.0f, 50.0f, 500.0f}),
        iris::RigidBodyType::STATIC);

    std::vector<std::tuple<
        std::uint32_t,
        iris::Vector3,
        std::unique_ptr<iris::PhysicsState, iris::PhysicsStateDeleter>>>
        history;
    std::uint32_t tick = 0u;

    ClientInput input;

    // keep looping till handshake and sync is complete which will give us a lag
    // estimate
    while (client.lag().count() == 0)
    {
        client.flush();
    }

    LOG_WARN("client", "lag: {}", client.lag().count());

    // calculate how many ticks we are behind the server based on lag
    const auto ticks_behind = (client.lag().count() / 33u) + 20u;
    LOG_INFO("client", "ticks behind {}", ticks_behind);

    // step ourselves forward in time
    // we want be a head of the server just enough so that it received input for
    // the frame it is about to execute
    for (auto i = 0u; i < ticks_behind; ++i)
    {
        ps.step(std::chrono::milliseconds(33));
    }

    tick = ticks_behind;

    iris::Looper looper(
        33ms * ticks_behind,
        33ms,
        [&](std::chrono::microseconds, std::chrono::microseconds) {
            // fixed timestep function
            // this simulates the same physics code as in the server, with the
            // same inputs

            auto keep_looping = false;

            // process user inputs
            if (!handle_input(inputs, camera, tick, client, window))
            {
                // apply inputs to physics simulation
                for (const auto &input : inputs)
                {
                    // only process inputs for this tick
                    if (input.tick == tick)
                    {
                        iris::Vector3 walk_direction{
                            input.side, 0.0f, input.forward};
                        walk_direction.normalise();

                        character_controller->set_walk_direction(
                            walk_direction);
                    }
                }

                ps.step(33ms);

                // snapshot the state of the simulation at this tick, this is
                // needed so we can rewind the state of we get out of sync with
                // the server
                history.emplace_back(
                    tick, character_controller->position(), ps.save());

                ++tick;
                keep_looping = true;
            }

            return keep_looping;
        },
        [&](std::chrono::microseconds, std::chrono::microseconds) {
            // variable timestep function
            // this handles various lag compensation techniques as well as
            // renders the world

            // process all pending messages from the server
            for (;;)
            {
                const auto server_data =
                    client.try_read(iris::ChannelType::RELIABLE_ORDERED);
                if (!server_data)
                {
                    break;
                }

                // process the message
                const auto
                    [last_acked,
                     server_position,
                     linear_velocity,
                     angular_velocity] =
                        process_server_update(
                            *server_data, snapshots, history, inputs);

                // if we have history then update the client prediction based
                // upon the latest information from the server
                if (!history.empty())
                {
                    client_prediciton(
                        last_acked,
                        history,
                        server_position,
                        linear_velocity,
                        angular_velocity,
                        character_controller,
                        inputs,
                        &ps);
                }
            }

            // put the camera where the player is
            camera.set_position(character_controller->position());

            // if we have snapshots then interpolate the server entity for
            // smooth motion
            if (snapshots.size() >= 2u)
            {
                entity_interpolation(snapshots, box);
            }

            // render the world
            window.render(pipeline);

            return true;
        });

    looper.run();

    LOG_ERROR("client", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
