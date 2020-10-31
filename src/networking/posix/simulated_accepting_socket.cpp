#include "networking/simulated_accepting_socket.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
#include <thread>
#include <tuple>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "core/root.h"
#include "jobs/concurrent_queue.h"
#include "networking/packet.h"
#include "networking/simulated_socket.h"

namespace iris
{

SimulatedAcceptingSocket::SimulatedAcceptingSocket(
    const std::string &client_queue_name,
    const std::string &server_queue_name,
    std::chrono::milliseconds delay,
    std::chrono::milliseconds jitter,
    float drop_rate)
    : client_(std::make_unique<SimulatedSocket>(
          client_queue_name,
          server_queue_name,
          delay,
          jitter,
          drop_rate))
{
}

Socket *SimulatedAcceptingSocket::accept()
{
    Socket *socket = nullptr;

    static auto once = false;
    if (!once)
    {
        socket = client_.get();
        once = true;
    }
    else
    {
        // sleep for a short while to simulate blocking, eventually we will
        // just return a nullptr
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return socket;
}

}
