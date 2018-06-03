#pragma once

#include "error_type.hpp"
#include "location_data.hpp"

namespace eng
{

/**
 * Class encapsulating a script error. Contains a type and location in the
 * script file.
 */
class error final
{
    public:

        /**
         * Construct a new error.
         *
         * @param location
         *   Location in script file of error.
         *
         * @param error_type
         *   Type of error.
         */
        error(
            const location_data location,
            const error_type);

        // default
        ~error() = default;
        error(const error&) = default;
        error& operator=(const error&) = default;
        error(error&&) = default;
        error& operator=(error&&) = default;

        /**
         * Get location of error.
         *
         * @returns
         *   Error location.
         */
        location_data location() const;

        /**
         * Get error type.
         *
         * @returns
         *   Type of error.
         */
        error_type type() const;

        /**
         * Equality operator. Tests if two error objects are the same.
         *
         * @param other
         *   Error object to compare with.
         *
         * @returns
         *   True if error objects are the same, false otherwise.
         */
        bool operator==(const error &other) const;

        /**
         * Inequality operator. Tests if two error objects are note the same.
         *
         * @param other
         *   Error object to compare with.
         *
         * @returns
         *   True if error objects are not the same, false otherwise.
         */
        bool operator!=(const error &other) const;

    private:

        /** Location of error in script. */
        location_data location_;

        /** Type of error. */
        error_type type_;
};

}

