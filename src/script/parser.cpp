#include "parser.hpp"

#include <memory>
#include <vector>

#include "expression.hpp"
#include "cast.hpp"
#include "panic_exception.hpp"
#include "script_exception.hpp"
#include "token.hpp"

/**
 * The script grammar:
 *
 * program -> declaration* ;
 *
 * declaration -> variable_declaration |
 *                function_declaration |
 *                statement ;
 *
 * variable_declaration -> "var" IDENTIFIER "=" expression ";" ;
 * function_declaration -> "function" IDENTIFIER "(" parameters? ")" block;
 * parameters           -> "var" IDENTIFIER ("," "var" IDENTIFIER)* ;
 *
 * statement -> expression_statement |
 *              if_statement |
 *              while_statement |
 *              do_while_statement |
 *              for_statement |
 *              for_each_statement |
 *              break_statement |
 *              return_statement |
 *              block ;
 *
 * expression_statement -> expression ";" ;
 * if_statement         -> "if" "(" equality ")" block ;
 * while_statement      -> "while" "(" equality ")" block ;
 * do_while_statement   -> "do" block "while" "(" equality ")" block ;
 * for_statement        -> "for" "(" variable_declaration equality ";" expression ")" block ;
 * for_each_statement   -> "for" "(" "var" IDENTIFIER "in" ")" block ;
 * break_statement      -> "break" ";" ;
 * return_statement     -> "return" expression? ";" ;
 * block                -> "{" declaration* "}" ;
 *
 * expression     -> assignment | object ;
 * assignment     -> IDENTIFIER ("[" expression "]")? "=" expression | equality ;
 * equality       -> comparison (("==" | "!=") comparison)* ;
 * comparison     -> addition (("<" | "<=" | ">" | ">=") addition)* ;
 * addition       -> multiplication (("+" | "-") multiplication)* ;
 * multiplication -> call (("*" | "/") call)* ;
 * call           -> base ("(" arguments? ")" | "[" expression "]")* ;
 * base           -> IDENTIFIER | primary | "(" expression ")";
 * arguments      -> expression "(" expression "," ")"* ;
 *
 * object -> "{" "}" | "{" member ("," member)* "}"
 * member -> expression ":" expression
 *
 * primary    -> NUMBER | STRING | "true" | "false";
 * NUMBER     -> DIGIT+ ( "." DIGIT+ )? ;
 * STRING     -> ''' <any char except '''>* ''' ;
 * IDENTIFIER -> ALPHA ( ALPHA | DIGIT )* ;
 * ALPHA      -> [a-zA-Z_] ;
 * DIGIT      -> [0-9] ;
 */


// convenient alias for an iterator into a collection of tokens
using token_cursor = std::vector<eng::token>::const_iterator;

namespace
{

/**
 * Helper function for consuming a token.
 *
 * Throws an exception if consuming the sentinel token.
 *
 * @param cursor
 *   Cursor into token collection to consume, this will be advanced after call
 *
 * @returns
 *   Token at cursor, before being advanced.
 */
eng::token consume(token_cursor &cursor)
{
    // throw an exception if we are at the last token
    if(eng::is<eng::token_type::END>(*cursor))
    {
        throw eng::panic_exception(eng::error_type::UNEXPECTED_END, *(cursor - 1u));
    }

    const auto t = *cursor;
    ++cursor;

    return t;
}

/**
 * Helper function for consuming a token, checking its type and if, it is not
 * expected, throwing a panic_exception.
 *
 * @param cursor
 *   Cursor to consume and check, will be advanced after call.
 *
 * @returns
 *   Token at cursor, before being advanced.
 */
template<eng::token_type Type>
eng::token expect(token_cursor &cursor)
{
    // check we are not at end and token is expected type
    if(eng::is<eng::token_type::END>(*cursor) || !eng::is<Type>(*cursor))
    {
        // report error on previous token as this will give more informative
        // errors
        const auto err_token = *(cursor - 1u);

        // throw a panic_exception with an error type appropriate to the
        // mismatched token
        switch(Type)
        {
            case eng::token_type::SEMICOLON:
                throw eng::panic_exception(eng::error_type::MISSING_SEMICOLON, err_token);
            case eng::token_type::COMMA:
                throw eng::panic_exception(eng::error_type::MISSING_COMMA, err_token);
            case eng::token_type::EQUALS:
                throw eng::panic_exception(eng::error_type::MISSING_EQUALS, err_token);
            case eng::token_type::IDENTIFIER:
                throw eng::panic_exception(eng::error_type::EXPECTED_IDENTIFIER, err_token);
            case eng::token_type::COLON:
                throw eng::panic_exception(eng::error_type::MISSING_COLON, err_token);
            case eng::token_type::LEFT_BRACE:
                throw eng::panic_exception(eng::error_type::MISSING_LEFT_BRACE, err_token);
            case eng::token_type::RIGHT_SQUARE:
                throw eng::panic_exception(eng::error_type::MISSING_RIGHT_SQUARE, err_token);
            case eng::token_type::LEFT_PAREN:
                throw eng::panic_exception(eng::error_type::MISSING_LEFT_PAREN, err_token);
            case eng::token_type::RIGHT_PAREN:
                throw eng::panic_exception(eng::error_type::MISSING_RIGHT_PAREN, err_token);
            case eng::token_type::WHILE:
                throw eng::panic_exception(eng::error_type::MISSING_WHILE, err_token);
            case eng::token_type::VAR:
                throw eng::panic_exception(eng::error_type::MISSING_VAR, err_token);
            case eng::token_type::IN:
                throw eng::panic_exception(eng::error_type::MISSING_IN, err_token);
            default:
                throw eng::panic_exception(eng::error_type::UNEXPECTED_SYMBOL, err_token);
        }
    }

    return consume(cursor);
}

// function declarations
std::unique_ptr<eng::expression> parse_expression(token_cursor &cursor);
std::unique_ptr<eng::statement> parse_declaration(token_cursor &cursor);
std::unique_ptr<eng::statement> parse_variable_declaration(token_cursor &cursor);

/**
 * Parse the following rule:
 * base -> IDENTIFIER | primary | "(" expression ")";
 *
 * As this is the last grammar rule this function will throw a panic_exception
 * if it cannot parse a base expression.
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_base(token_cursor &cursor)
{
    std::unique_ptr<eng::expression> expr = nullptr;
    const auto token = consume(cursor);

    if(eng::is<eng::token_type::IDENTIFIER>(token))
    {
        expr = std::make_unique<eng::variable_expression>(token);
    }
    else if(eng::is<
                eng::token_type::NUMBER,
                eng::token_type::STRING,
                eng::token_type::TRUE,
                eng::token_type::FALSE>(token))
    {
        expr = std::make_unique<eng::literal_expression>(token);
    }
    else if(eng::is<eng::token_type::LEFT_PAREN>(token))
    {
        expr = parse_expression(cursor);
        expect<eng::token_type::RIGHT_PAREN>(cursor);
    }
    else
    {
        throw eng::panic_exception(eng::error_type::UNEXPECTED_SYMBOL, token);
    }

    return expr;
}

/**
 * Helper function to finish parsing a subscript expression.
 *
 * @param object
 *   The object being subscripted.
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> finish_subscript(
    std::unique_ptr<eng::expression> &&object,
    token_cursor &cursor)
{
    auto member = parse_expression(cursor);

    const auto token = expect<eng::token_type::RIGHT_SQUARE>(cursor);
    return std::make_unique<eng::get_expression>(
        token,
        std::move(object),
        std::move(member));
}

/**
 * Helper function to finish parsing a call expression.
 *
 * @param callee
 *   The expression being called.
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> finish_call(
    std::unique_ptr<eng::expression> &&callee,
    token_cursor &cursor)
{
    std::vector<std::unique_ptr<eng::expression>> args{ };

    // we could have 0 or more parameters, so keep looping until we get a right
    // paren
    while(!eng::is<eng::token_type::RIGHT_PAREN>(*cursor))
    {
        args.emplace_back(parse_expression(cursor));

        // if next token is not a right paren then expect a comma
        if(!eng::is<eng::token_type::RIGHT_PAREN>(*cursor))
        {
            expect<eng::token_type::COMMA>(cursor);
        }
    }

    const auto token = expect<eng::token_type::RIGHT_PAREN>(cursor);
    return std::make_unique<eng::call_expression>(
        token,
        std::move(callee),
        std::move(args));
}

/**
 * Parse the following rule:
 * call -> base ("(" arguments? ")" | "[" expression "]")* ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_call(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_base(cursor);

    // we could have 0 or more function or subscript operators, for example
    // func()[]()[]
    // so we keep looping until the next token is not right paren or right
    // square
    for(;;)
    {
        if(eng::is<eng::token_type::LEFT_PAREN>(*cursor))
        {
            consume(cursor);

            // finish handling call, note we move the current expression in,
            // this allows us to set the function caller to the already parsed
            // expression
            expr = finish_call(std::move(expr), cursor);
        }
        else if(eng::is<eng::token_type::LEFT_SQUARE>(*cursor))
        {
            consume(cursor);

            // finish handling subscript, note we move the current expression in,
            // this allows us to set the subscript object to the already parsed
            // expression
            expr = finish_subscript(std::move(expr), cursor);
        }
        else
        {
            // not a right paren or right square so break out of loop
            break;
        }
    }

    return expr;
}

/**
 * Parse the following rule:
 * multiplication -> call (("*" | "/") call)* ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_multiplication(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_call(cursor);

    // keep parsing star an slash binop expressions
    while(eng::is<eng::token_type::STAR, eng::token_type::SLASH>(*cursor))
    {
        const auto op = consume(cursor);

        expr = std::make_unique<eng::binop_expression>(
            std::move(expr),
            op,
            parse_call(cursor));
    }

    return expr;
}

/**
 * Parse the following rule:
 * addition -> multiplication (("+" | "-") multiplication)* ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_addition(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_multiplication(cursor);

    // keep parsing plus and minus bionp expressions
    while(eng::is<eng::token_type::PLUS, eng::token_type::MINUS>(*cursor))
    {
        const auto op = consume(cursor);

        expr = std::make_unique<eng::binop_expression>(
            std::move(expr),
            op,
            parse_multiplication(cursor));
    }

    return expr;
}

/**
 * Parse the following rule:
 * comparison -> addition (("<" | "<=" | ">" | ">=") addition)* ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_comparison(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_addition(cursor);

    // keep parsing comparison binop expressions
    while(eng::is<
            eng::token_type::EQUAL_EQUAL,
            eng::token_type::BANG_EQUAL,
            eng::token_type::LESS,
            eng::token_type::LESS_EQUAL,
            eng::token_type::MORE,
            eng::token_type::MORE_EQUAL>(*cursor))
    {
        const auto op = consume(cursor);

        expr = std::make_unique<eng::binop_expression>(
            std::move(expr),
            op,
            parse_addition(cursor));
    }

    return expr;
}

/**
 * Parse the following rule:
 * equality -> comparison (("==" | "!=") comparison)* ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_equality(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_comparison(cursor);

    // keep parsing comparison binop expressions
    while(eng::is<eng::token_type::EQUAL_EQUAL, eng::token_type::BANG_EQUAL>(*cursor))
    {
        const auto op = consume(cursor);

        expr = std::make_unique<eng::binop_expression>(
            std::move(expr),
            op,
            parse_comparison(cursor));
    }

    return expr;
}

/**
 * Parse the following rule:
 * assignment -> IDENTIFIER ("[" expression "]")? "=" expression | equality ;
 *
 * Throws a panic_exception if attempting an invalid assignment.
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_assignment(token_cursor &cursor)
{
    // parse next rule down
    auto expr = parse_equality(cursor);

    // if next symbol is an equals then we have an assignment
    if(eng::is<eng::token_type::EQUALS>(*cursor))
    {
        const auto equals = consume(cursor);
        auto value = parse_expression(cursor);

        if(auto *var = eng::dyn_cast<eng::variable_expression>(expr) ; var != nullptr)
        {
            // if our parsed expression was a variable then we have an
            // assignment expression
            expr = std::make_unique<eng::assignment_expression>(
                var->tok,
                std::move(value));
        }
        else if(auto *get = eng::dyn_cast<eng::get_expression>(expr) ; get != nullptr)
        {
            // if our parsed expression was a get expression then we actually
            // have a set expression
            expr = std::make_unique<eng::set_expression>(
                equals,
                std::move(get->object),
                std::move(get->member),
                std::move(value));
        }
        else
        {
            throw eng::panic_exception(eng::error_type::INVALID_ASSIGNMENT, *(cursor - 1));
        }
    }

    return expr;
}

/**
 * Parse the following rule:
 * object -> "{" "}" | "{" member ("," member)* "}"
 * member -> expression ":" expression
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_object(token_cursor &cursor)
{
    // parse next rule down
    const auto token = consume(cursor);

    std::vector<std::pair<std::unique_ptr<eng::expression>, std::unique_ptr<eng::expression>>> members;

    // keep looping whilst we don't have a right brace
    while(!eng::is<eng::token_type::RIGHT_BRACE>(*cursor))
    {
        auto key = parse_expression(cursor);
        expect<eng::token_type::COLON>(cursor);
        auto value = parse_expression(cursor);

        // if next token is a comma then consume it
        if(eng::is<eng::token_type::COMMA>(*cursor))
        {
            consume(cursor);
        }

        members.emplace_back(std::make_pair(std::move(key), std::move(value)));
    }

    // consume right brace
    consume(cursor);

    return std::make_unique<eng::object_expression>(token, std::move(members));
}

/**
 * Parse the following rule:
 * expression -> assignment | object ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed expression.
 */
std::unique_ptr<eng::expression> parse_expression(token_cursor &cursor)
{
    return eng::is<eng::token_type::LEFT_BRACE>(*cursor)
        ? parse_object(cursor)
        : parse_assignment(cursor);
}

/**
 * Parse the following rule:
 * block -> "{" declaration* "}" ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed statement.
 */
std::unique_ptr<eng::statement> parse_block(token_cursor &cursor)
{
    expect<eng::token_type::LEFT_BRACE>(cursor);

    std::vector<std::unique_ptr<eng::statement>> statements{ };

    // keep looping until right brace
    while(!eng::is<eng::token_type::RIGHT_BRACE>(*cursor))
    {
        // throw an exception if we reach end of tokens without finding a right
        // brace
        if(eng::is<eng::token_type::END>(*cursor))
        {
            throw eng::panic_exception(eng::error_type::MISSING_RIGHT_BRACE, *(cursor - 1u));
        }

        statements.emplace_back(parse_declaration(cursor));
    }

    const auto right_brace = consume(cursor);

    return std::make_unique<eng::block_statement>(right_brace, std::move(statements));
}

/**
 * Parse the following rule:
 * statement -> expression_statement |
 *              if_statement |
 *              while_statement |
 *              do_while_statement |
 *              for_statement |
 *              for_each_statement |
 *              break_statement |
 *              return_statement |
 *              block ;
 *
 * expression_statement -> expression ";" ;
 * if_statement         -> "if" "(" equality ")" block ;
 * while_statement      -> "while" "(" equality ")" block ;
 * do_while_statement   -> "do" block "while" "(" equality ")" block ;
 * for_statement        -> "for" "(" variable_declaration equality ";" expression ")" block ;
 * for_each_statement   -> "for" "(" "var" IDENTIFIER "in" ")" block ;
 * break_statement      -> "break" ";" ;
 * return_statement     -> "return" expression? ";" ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed statement.
 */
std::unique_ptr<eng::statement> parse_statement(token_cursor &cursor)
{
    std::unique_ptr<eng::statement> stmtn = nullptr;

    // the statement type is defined by the next token

    if(eng::is<eng::token_type::LEFT_BRACE>(*cursor))
    {
        stmtn = parse_block(cursor);
    }
    else if(eng::is<eng::token_type::IF>(*cursor))
    {
        const auto token = consume(cursor);
        expect<eng::token_type::LEFT_PAREN>(cursor);
        auto condition = parse_equality(cursor);
        expect<eng::token_type::RIGHT_PAREN>(cursor);

        auto branch = parse_block(cursor);

        stmtn = std::make_unique<eng::if_statement>(
            token,
            std::move(condition),
            std::move(branch));
    }
    else if(eng::is<eng::token_type::WHILE>(*cursor))
    {
        const auto token = consume(cursor);
        expect<eng::token_type::LEFT_PAREN>(cursor);
        auto condition = parse_equality(cursor);
        expect<eng::token_type::RIGHT_PAREN>(cursor);

        auto branch = parse_block(cursor);

        stmtn = std::make_unique<eng::while_statement>(
            token,
            std::move(condition),
            std::move(branch));
    }
    else if(eng::is<eng::token_type::DO>(*cursor))
    {
        const auto token = consume(cursor);
        auto branch = parse_block(cursor);

        expect<eng::token_type::WHILE>(cursor);
        expect<eng::token_type::LEFT_PAREN>(cursor);
        auto condition = parse_equality(cursor);
        expect<eng::token_type::RIGHT_PAREN>(cursor);

        stmtn = std::make_unique<eng::do_while_statement>(
            token,
            std::move(condition),
            std::move(branch));
    }
    else if(eng::is<eng::token_type::FOR>(*cursor))
    {
        const auto token = consume(cursor);
        expect<eng::token_type::LEFT_PAREN>(cursor);

        auto init = parse_variable_declaration(cursor);
        auto condition = parse_equality(cursor);
        expect<eng::token_type::SEMICOLON>(cursor);

        auto modifier = parse_expression(cursor);

        expect<eng::token_type::RIGHT_PAREN>(cursor);

        auto branch = parse_block(cursor);

        stmtn = std::make_unique<eng::for_statement>(
            token,
            std::move(init),
            std::move(condition),
            std::move(modifier),
            std::move(branch));
    }
    else if(eng::is<eng::token_type::FOREACH>(*cursor))
    {
        consume(cursor);
        expect<eng::token_type::LEFT_PAREN>(cursor);
        expect<eng::token_type::VAR>(cursor);

        const auto iterator = expect<eng::token_type::IDENTIFIER>(cursor);

        expect<eng::token_type::IN>(cursor);

        auto iterable = parse_expression(cursor);

        expect<eng::token_type::RIGHT_PAREN>(cursor);

        auto branch = parse_block(cursor);

        stmtn = std::make_unique<eng::for_each_statement>(
            iterator,
            std::move(iterable),
            std::move(branch));
    }
    else if(eng::is<eng::token_type::BREAK>(*cursor))
    {
        const auto tok = consume(cursor);
        expect<eng::token_type::SEMICOLON>(cursor);

        stmtn = std::make_unique<eng::break_statement>(tok);
    }
    else if(eng::is<eng::token_type::RETURN>(*cursor))
    {
        const auto tok = consume(cursor);

        std::unique_ptr<eng::expression> value = std::make_unique<eng::literal_expression>(
            eng::token(eng::token_type::NUMBER, "0.0f", { 0u, 0u }));

        // handle case where we are returning an explicit expression
        if(!eng::is<eng::token_type::SEMICOLON>(*cursor))
        {
            value = parse_expression(cursor);
        }

        expect<eng::token_type::SEMICOLON>(cursor);

        stmtn = std::make_unique<eng::return_statement>(
            tok,
            std::move(value));
    }
    else
    {
        // the base case is an expression statement
        //
        auto expr = parse_expression(cursor);
        const auto token = expr->tok;
        stmtn = std::make_unique<eng::expression_statement>(token, std::move(expr));
        expect<eng::token_type::SEMICOLON>(cursor);
    }

    return stmtn;
}

/**
 * Parse the following rule:
 * variable_declaration -> "var" IDENTIFIER "=" expression ";" ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed statement.
 */
std::unique_ptr<eng::statement> parse_variable_declaration(token_cursor &cursor)
{
    // first token should be var
    expect<eng::token_type::VAR>(cursor);

    const auto identifier = expect<eng::token_type::IDENTIFIER>(cursor);
    const auto equals = expect<eng::token_type::EQUALS>(cursor);

    auto expr = parse_expression(cursor);
    expect<eng::token_type::SEMICOLON>(cursor);

    return std::make_unique<eng::variable_statement>(
        identifier,
        std::move(expr));
}

/**
 * Parse the following rule:
 * function_declaration -> "function" IDENTIFIER "(" parameters? ")" block;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed statement.
 */
std::unique_ptr<eng::statement> parse_function_declaration(token_cursor &cursor)
{
    std::vector<eng::token> parameters{ };

    // first token should be function
    expect<eng::token_type::FUNCTION>(cursor);

    const auto name = expect<eng::token_type::IDENTIFIER>(cursor);
    expect<eng::token_type::LEFT_PAREN>(cursor);

    // keep looping until we get a right paren
    while(!eng::is<eng::token_type::RIGHT_PAREN>(*cursor))
    {
        expect<eng::token_type::VAR>(cursor);
        const auto param = expect<eng::token_type::IDENTIFIER>(cursor);
        parameters.emplace_back(param);

        // if we are not at the right paren then we should have a comma
        if(!eng::is<eng::token_type::RIGHT_PAREN>(*cursor))
        {
            expect<eng::token_type::COMMA>(cursor);
        }
    }

    expect<eng::token_type::RIGHT_PAREN>(cursor);
    auto block = parse_block(cursor);

    // flag for keeping track of whether we need to add a return to the end of
    // the function block (if one wasn't present in the script)
    auto add_ret = false;

    auto *block_stmnt = static_cast<eng::block_statement*>(block.get());

    // if we have a non-empty function block
    if(!block_stmnt->statements.empty())
    {
        // if the last statement was not a return statement then we need to add
        // one
        const auto *last_stmnt = block_stmnt->statements.back().get();
        if(!eng::is<eng::token_type::RETURN>(last_stmnt->tok))
        {
            add_ret = true;
        }
    }
    else
    {
        // we also need to add one if the function block was empty
        add_ret = true;
    }

    // add a return statement to the end of the function block (if needed)
    // default behaviour is to return 0.0 (float)
    if(add_ret)
    {
        eng::token ret_tok{ eng::token_type::RETURN, { 0u, 0u } };
        eng::token val_tok{ eng::token_type::NUMBER, "0.0", { 0u, 0u } };
        auto lit_expr = std::make_unique<eng::literal_expression>(val_tok);

        block_stmnt->statements.emplace_back(
            std::make_unique<eng::return_statement>(ret_tok, std::move(lit_expr)));
    }

    return std::make_unique<eng::function_statement>(
        name,
        std::move(parameters),
        std::move(block));
}

/**
 * Parse the following rule:
 * declaration -> variable_declaration |
 *                function_declaration |
 *                statement ;
 *
 * @param cursor
 *   Cursor into tokens.
 *
 * @returns
 *   Parsed statement.
 */
std::unique_ptr<eng::statement> parse_declaration(token_cursor &cursor)
{
    std::unique_ptr<eng::statement> statement = nullptr;

    if(eng::is<eng::token_type::VAR>(*cursor))
    {
        statement = parse_variable_declaration(cursor);
    }
    else if(eng::is<eng::token_type::FUNCTION>(*cursor))
    {
        statement = parse_function_declaration(cursor);
    }
    else
    {
        statement = parse_statement(cursor);
    }

    return statement;
}

}

namespace eng
{

parser::parser(const std::vector<token> &tokens)
    : statements_()
{
    std::vector<error> errors{ };

    auto cursor = std::cbegin(tokens);

    // keep parsing tokens until we reach the end
    while(cursor->type != token_type::END)
    {
        // silently consume empty statements i.e. just a semicolon
        if(is<token_type::SEMICOLON>(*cursor))
        {
            consume(cursor);
            continue;
        }

        // here we will try and parse each declaration
        // if a panic_exception is thrown then we will 'synchronise' and
        // continue parsing, this allows us a chance to report as many errors as
        // possible
        // to synchronise we will consume as many tokens as needed to get to
        // a point where we think we can continue parsing, without the current
        // error interfering, this is a best guess effort and can lead to
        // cascading errors, at least the first error should be correct!

        try
        {
            // try and parse declaration
            statements_.emplace_back(parse_declaration(cursor));
        }
        catch(const panic_exception &e)
        {
            // caught a panic exception so store the error and synchronise

            const auto token = e.bad_token();
            const auto location = token.location;
            errors.emplace_back(
                location_data{ location.line, location.offset + token.lexeme.length() },
                e.type());

            // keep consuming tokens until we reach the end or a semicolon
            // a semicolon is a good synchronisation point as it is probably
            // the last token before a new statement starts
            while(!is<token_type::END, token_type::SEMICOLON>(*cursor))
            {
                consume(cursor);
            }
        }
        catch(const exception&)
        {
            // allow regular engine exceptions to propagate
            throw;
        }
        catch(...)
        {
            // any other exception is an internal parser error, so report it
            // as such
            throw exception("internal parser error");
        }
    }

    // if we encountered any errors then propagate them as an exception
    if(!errors.empty())
    {
        throw script_exception(errors);
    }
}

std::vector<std::unique_ptr<statement>> parser::yield()
{
    return std::move(statements_);
}

}

