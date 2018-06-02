#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "error.hpp"
#include "token.hpp"
#include "statement.hpp"

namespace eng
{

/**
 * This class consumes a collection of tokens and produces and AST. This is done
 * with a recursive decent parser.
 */
class parser final
{
    public:

        /**
         * Construct a new parser.
         *
         * @param tokens
         *   Tokens to parse into AST.
         */
        parser(const std::vector<token> &tokens);

        // default
        ~parser() = default;
        parser(const parser&) = default;
        parser& operator=(const parser&) = default;
        parser(parser&&) = default;
        parser& operator=(parser&&) = default;

        /**
         * Yield the collection of parsed statements, this is the AST for the
         * parsed program.
         *
         * @returns
         *   Collection of parsed statements.
         */
        std::vector<std::unique_ptr<statement>> yield();

    private:

        /** Parsed statements. */
        std::vector<std::unique_ptr<statement>> statements_;

};

}

