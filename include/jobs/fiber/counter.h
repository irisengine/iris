#pragma once

#include <atomic>
#include <mutex>

namespace eng
{

/**
 * A thread-safe counter. Can be decremented and checked.
 */
class Counter
{
    public:
        /**
         * Construct counter with initial value.
         *
         * @param value
         *   Initial value of counter.
         */
        explicit Counter(int value);

        // disable copy and move
        Counter(const Counter&) = delete;
        Counter& operator=(const Counter&) = delete;
        Counter(Counter&&) = delete;
        Counter& operator=(Counter&&) = delete;

        /**
         * Cast counter value to int. 
         *
         * @returns
         *   Value of counter.
         */
        operator int();

        /**
         * Prefix decrement counter.
         */
        void operator--();

        /**
         * Postfix decrement counter.
         */
        void operator--(int);

    private:

        /** Value of counter. */
        int value_;

        /** Lock for object. */
        std::mutex mutex_;
};

}

