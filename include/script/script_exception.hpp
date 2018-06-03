#pragma once

#include <vector>

#include "error.hpp"
#include "exception.hpp"

namespace eng
{

/**
 * An exception that can propagate a collection of script errors up. Should be
 * used when any stage of script processing encounters an error.
 */
class script_exception final : public exception
{
    public:

        /**
         * Construct a new error_exception with a single error.
         *
         * @param err
         *   Error to propagate.
         */
        script_exception(const error &err);

        /**
         * Construct a new error_exception with a collection of errors.
         *
         * @param errors
         *   Collection of errors to propagate.
         */
        script_exception(const std::vector<error> &errors);

        ~script_exception() override = default;
        script_exception(const script_exception&) = default;
        script_exception& operator=(const script_exception&) = default;
        script_exception(script_exception&&) = default;
        script_exception& operator=(script_exception&&) = default;

        /**
         * Get errors.
         *
         * @returns
         *   Errors.
         */
        std::vector<error> errors() const;

    private:

        /** Collection of errors. */
        std::vector<error> errors_;
};

}

