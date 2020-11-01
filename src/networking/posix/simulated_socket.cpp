#include "networking/simulated_socket.h"

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <random>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <thread>

#include "core/root.h"
#include "jobs/concurrent_queue.h"
#include "log/log.h"

namespace
{

/**
 * Struct for an IPC message.
 */
struct IpcMessage
{
    long mtype;
    char mtext[1024];
};

/**
 * Helper function to flush all messages from an IPC queue.
 *
 * @param queue
 *   Handle to queue to flush.
 */
void flush_queue(int queue)
{
    auto flushing = true;
    do
    {
        IpcMessage message{};
        if (::msgrcv(
                queue,
                &message,
                sizeof(message.mtext),
                0,
                MSG_NOERROR | IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                throw iris::Exception("read failed");
            }

            // we can stop when a non-blocking read reports there are no
            // messages on the queue
            flushing = false;
        }

    } while (flushing);
}

}

namespace iris
{

struct SimulatedSocket::implementation
{
    int client_queue = -1;
    int server_queue = -1;
    ConcurrentQueue<
        std::tuple<DataBuffer, std::chrono::steady_clock::time_point>>
        write_queue;
    std::mutex mutex;
    std::condition_variable signal;
};

SimulatedSocket::SimulatedSocket(
    const std::string &client_queue_name,
    const std::string &server_queue_name,
    std::chrono::milliseconds delay,
    std::chrono::milliseconds jitter,
    float drop_rate)
    : impl_(std::make_unique<implementation>())
    , delay_(delay)
    , jitter_(jitter)
    , drop_rate_(drop_rate)
{
    impl_->client_queue =
        ::msgget(std::stoi(client_queue_name), IPC_CREAT | 0666);
    if (impl_->client_queue == -1)
    {
        throw Exception("failed to open client queue");
    }

    impl_->server_queue =
        ::msgget(std::stoi(server_queue_name), IPC_CREAT | 0666);
    if (impl_->server_queue == -1)
    {
        throw Exception("failed to open server queue");
    }

    // there may be messages in the queue from previous executions (especially
    // if they did not end gracefully) flush any old messages
    flush_queue(impl_->client_queue);
    flush_queue(impl_->server_queue);

    // in order to facilitate message delay without blocking we have write()
    // enqueue data with a time point, this job then grabs them and can wait
    // until the delay has passed before sending
    Root::job_system().add_jobs({[this]() {
        for (;;)
        {
            std::tuple<DataBuffer, std::chrono::steady_clock::time_point>
                element;

            {
                // block and wait for write() call to put some data on the
                // send queue

                std::unique_lock lock(impl_->mutex);
                impl_->signal.wait(lock, [this]() {
                    return !impl_->write_queue.empty();
                });

                element = impl_->write_queue.dequeue();
            }

            const auto &[buffer, time_point] = element;

            // wait until its time to send the data
            std::this_thread::sleep_until(time_point);

            // package up data in IPC message and send
            IpcMessage message{};
            message.mtype = 1;
            std::memcpy(message.mtext, buffer.data(), buffer.size());

            if (::msgsnd(impl_->server_queue, &message, buffer.size(), 0) == -1)
            {
                throw Exception("failed to write");
            }
        }
    }});
}

SimulatedSocket::~SimulatedSocket()
{
    // delete the queues
    ::msgctl(impl_->client_queue, IPC_RMID, nullptr);
    ::msgctl(impl_->server_queue, IPC_RMID, nullptr);
}

std::optional<DataBuffer> SimulatedSocket::try_read(std::size_t count)
{
    std::optional<DataBuffer> out;

    // try and read an IPC message
    IpcMessage message{};
    const auto read = ::msgrcv(
        impl_->client_queue,
        &message,
        sizeof(message.mtext),
        0,
        MSG_NOERROR | IPC_NOWAIT);
    if (read == -1)
    {
        if (errno != ENOMSG)
        {
            throw Exception("read failed");
        }
    }
    else
    {
        // copy data out of the IPC message
        const auto *data = reinterpret_cast<std::byte *>(message.mtext);
        out = DataBuffer(data, data + read);
    }

    return out;
}

DataBuffer SimulatedSocket::read(std::size_t)
{
    // read an IPC message
    IpcMessage message{};
    const auto read = ::msgrcv(
        impl_->client_queue, &message, sizeof(message.mtext), 0, MSG_NOERROR);
    if (read == -1)
    {
        throw Exception("read failed");
    }

    // copy data out of the IPC message
    const auto *data = reinterpret_cast<std::byte *>(message.mtext);
    return {data, data + read};
}

void SimulatedSocket::write(const DataBuffer &buffer)
{
    if (buffer.size() > sizeof(IpcMessage::mtext))
    {
        throw Exception("trying to send too much data");
    }

    // setup RNG
    std::random_device rd;
    std::mt19937 gen(rd());

    // create distributions
    std::uniform_int_distribution<> jitter_distribution(
        -jitter_.count(), jitter_.count());
    std::bernoulli_distribution drop_distribution(drop_rate_);

    if (!drop_distribution(gen))
    {
        // stick the data to be sent on the queue (with the delay time) and
        // notify the send job
        const auto delay =
            delay_ + std::chrono::milliseconds(jitter_distribution(gen));
        impl_->write_queue.enqueue(
            buffer, std::chrono::steady_clock::now() + delay);
        impl_->signal.notify_one();
    }
    else
    {
        LOG_WARN("sim_sock", "dropping packet");
    }
}

void SimulatedSocket::write(const std::byte *data, std::size_t size)
{
    write({data, data + size});
}

}
