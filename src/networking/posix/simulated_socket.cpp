#include "networking/simulated_socket.h"

#include <chrono>
#include <cstddef>
#include <optional>
#include <random>
#include <thread>

#include "core/random.h"
#include "core/root.h"
#include "jobs/concurrent_queue.h"
#include "jobs/job_system.h"
#include "log/log.h"

using namespace std::chrono_literals;

namespace iris
{

SimulatedSocket::SimulatedSocket(
    std::chrono::milliseconds delay,
    std::chrono::milliseconds jitter,
    float drop_rate,
    Socket *socket)
    : delay_(delay)
    , jitter_(jitter)
    , drop_rate_(drop_rate)
    , socket_(socket)
{
    // in order to facilitate message delay without blocking we have write()
    // enqueue data with a time point, this job then grabs them and can wait
    // until the delay has passed before sending
    Root::job_system().add_jobs({[this]() {
        for (;;)
        {
            if (write_queue_.empty())
            {
                std::this_thread::sleep_for(10ms);
            }
            else
            {
                const auto &[buffer, time_point] = write_queue_.dequeue();

                // wait until its time to send the data
                std::this_thread::sleep_until(time_point);

                socket_->write(buffer);
            }
        }
    }});
}

SimulatedSocket::~SimulatedSocket() = default;

std::optional<DataBuffer> SimulatedSocket::try_read(std::size_t count)
{
    return socket_->try_read(count);
}

DataBuffer SimulatedSocket::read(std::size_t count)
{
    return socket_->read(count);
}

void SimulatedSocket::write(const DataBuffer &buffer)
{
    if (!flip_coin(drop_rate_))
    {
        const auto jitter = random_int32(
            static_cast<std::int32_t>(-jitter_.count()),
            static_cast<std::int32_t>(jitter_.count()));

        // stick the data to be sent on the queue (with the delay time) and
        const auto delay = delay_ + std::chrono::milliseconds(jitter);
        write_queue_.enqueue(buffer, std::chrono::steady_clock::now() + delay);
    }
}

void SimulatedSocket::write(const std::byte *data, std::size_t size)
{
    write({data, data + size});
}

}
