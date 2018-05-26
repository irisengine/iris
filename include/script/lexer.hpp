#pragma once

#include <string_view>
#include <vector>

#include "token.hpp"

namespace eng
{

/**
 * This class takes a script source and lexes it into a collection of tokens.
 */
class lexer final
{
    public:

        /**
         * Construct a new lexer class with script source.
         *
         * @param source
         *   The script source to lex.
         */
        lexer(std::string_view source);

        /**
         * Get the lexed tokens.
         *
         * @returns
         *   Collection of lexed tokens/
         */
        std::vector<token> tokens() const;

    private:

        /** Collection of lexed tokens. */
        std::vector<token> tokens_;

};

}

