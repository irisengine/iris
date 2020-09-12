#pragma once

namespace iris
{

// forward declaration
class Socket;

/**
 * Interface for an accepting socket. This is an object that listens for
 * connections and creates Socket objects for them so they may be communicated
 * with.
 */
class AcceptingSocket
{
    public:

        // default
        virtual ~AcceptingSocket() = default;

        /**
         * Block and wait for a new connection.
         * 
         * @returns
         *   A Socket object for communicating with the new connection.
         */
        virtual Socket* accept() = 0;
};

}
