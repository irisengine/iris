#pragma once

#include "error.hpp"
#include "exception.hpp"
#include "token.hpp"

namespace eng
{

/**
 * An exception for synchronising part of the scripting compilation process.
 * This should be thrown when a syntatic or semantic error is encountered. The
 * current stage of the compiler should internally catch this, synchronise
 * itself then continue processing. This allows for as many errors to be
 * reported as possible.
 */
class panic_exception final : public exception
{
    public:

        /**
         * Construct a new panic_exception.
         *
         * @param type
         *   Type of error.
         *
         * @param bad_token
         *   Token that caused the error.
         */
        panic_exception(const error_type type, const token &bad_token);

        // default
        ~panic_exception() override = default;
        panic_exception(const panic_exception&) = default;
        panic_exception& operator=(const panic_exception&) = default;
        panic_exception(panic_exception&&) = default;
        panic_exception& operator=(panic_exception&&) = default;

        /**
         * Get error type.
         *
         * @returns
         *   Type of error.
         */
        error_type type() const;

        /**
         * Get token that caused error.
         *
         * @returns
         *   Error token.
         */
        token bad_token() const;

    private:

        /** Type of error. */
        error_type error_type_;

        /** Offending token. */
        token bad_token_;
};

}

