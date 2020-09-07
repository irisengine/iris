#pragma once

#include <memory>
#include <utility>

#include <unistd.h>

namespace eng
{

/**
 * RAII class to automatically close a posix socket when it goes out of scope.
 */
class AutoSocket
{
    public:

        /**
         * Construct a new AutoSocket.
         */
        AutoSocket()
            : AutoSocket(-1)
        { }

        /**
         * Construct an AutoSocket which takes ownership of a posix socket
         * handle.
         * 
         * @param fd
         *   Posix socket handle to take ownership of.
         */
        AutoSocket(int fd)
            : fd_(fd)
        { }

        /**
         * Destructor, closes socket.
         */
        ~AutoSocket()
        {
            if(fd_ >= 0)
            {
                ::close(fd_);
            }
        }

        // disabled
        AutoSocket(const AutoSocket&) = delete;
        AutoSocket& operator=(const AutoSocket&) = delete;

        /**
         * Move constructor, steals the socket from the moved in object.
         * 
         * @param other
         *   Object to move from.
         */
        AutoSocket(AutoSocket &&other)
            : fd_(-1)
        {
            swap(other);
        }

        /**
         * Move assignment, steals the socket from the moved in object.
         * 
         * @param other
         *   Object to move from.
         * 
         * @returns
         *   Reference to this object after move.
         */
        AutoSocket& operator=(AutoSocket &&other)
        {
            AutoSocket new_auto_socket{ std::move(other) };
            swap(new_auto_socket);

            return *this;
        }

        /**
         * Swap this object with another.
         * 
         * @param other
         *   Object to swap with.
         */
        void swap(AutoSocket &other)
        {
            std::swap(fd_, other.fd_);
        }

        /**
         * Cast operator
         * 
         * @returns
         *   Socket file descriptor.
         */
        operator int() const
        {
            return fd_;
        }

    private:

        /** Posix file descriptor. */
        int fd_;
};

}

