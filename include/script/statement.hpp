#pragma once

#include <memory>
#include <vector>

#include "expression.hpp"
#include "statement_type.hpp"
#include "statement_visitor.hpp"
#include "token.hpp"

namespace eng
{

/**
 * Base struct for an statement object. This stores a token as all statements
 * come from some token. If an error occurs during at somepoint during
 * compilation and emulation then this token is used to generate error
 * information.
 */
struct statement
{
    /**
     * Construct a new statement object.
     *
     * @param type
     *   The type of statement.
     *
     * @param tok
     *   The token that forms this statement.
     */
    statement(statement_type type, token tok)
        : type(type),
          tok(tok)
    { }

    // default
    virtual ~statement() = default;
    statement(const statement&) = default;
    statement& operator=(const statement&) = default;
    statement(statement&&) = default;
    statement& operator=(statement&&) = default;

    /**
     * Accept a visitor.
     */
    virtual void accept(statement_visitor&) = 0;

    statement_type type;

    token tok;
};

/**
 * Statement for variable declaration.
 */
struct variable_statement final : public statement
{
    /**
     * Construct a new variable_statement.
     *
     * @param name
     *   Name of variable.
     *
     * @param value
     *   The expression to assign.
     */
    variable_statement(token name, std::unique_ptr<expression> value)
        : statement(statement_type::VARIABLE, name),
          value(std::move(value))
    { }

    // default
    ~variable_statement() override = default;
    variable_statement(const variable_statement&) = default;
    variable_statement& operator=(const variable_statement&) = default;
    variable_statement(variable_statement&&) = default;
    variable_statement& operator=(variable_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::VARIABLE;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Value to assign to variable. */
    std::unique_ptr<expression> value;
};

/**
 * Statement for an expression.
 */
struct expression_statement final : public statement
{
    /**
     * Construct a new expression_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param expr
     *   Expression.
     */
    expression_statement(token tok, std::unique_ptr<expression> expr)
        : statement(statement_type::EXPRESSION, tok),
          expr(std::move(expr))
    { }

    // default
    ~expression_statement() override = default;
    expression_statement(const expression_statement&) = default;
    expression_statement& operator=(const expression_statement&) = default;
    expression_statement(expression_statement&&) = default;
    expression_statement& operator=(expression_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::EXPRESSION;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Expression. */
    std::unique_ptr<expression> expr;
};

/**
 * Statement for a block.
 */
struct block_statement final : public statement
{
    /**
     * Construct a new block_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param statementes.
     *   Collection of statementes in block.
     */
    block_statement(token tok, std::vector<std::unique_ptr<statement>> &&statements)
        : statement(statement_type::BLOCK, tok),
          statements(std::move(statements))
    { }

    // default
    ~block_statement() override = default;
    block_statement(const block_statement&) = default;
    block_statement& operator=(const block_statement&) = default;
    block_statement(block_statement&&) = default;
    block_statement& operator=(block_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::BLOCK;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Collection of statements. */
    std::vector<std::unique_ptr<statement>> statements;
};

/**
 * If statement.
 */
struct if_statement final : public statement
{
    /**
     * Construct a new if_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param condition
     *   Expression for if condition.
     *
     * @param branch
     *   Statement to execute if condition is true.
     */
    if_statement(
        token tok,
        std::unique_ptr<expression> &&condition,
        std::unique_ptr<statement> &&branch)
        : statement(statement_type::IF, tok),
          condition(std::move(condition)),
          branch(std::move(branch))
    { }

    // default
    ~if_statement() override = default;
    if_statement(const if_statement&) = default;
    if_statement& operator=(const if_statement&) = default;
    if_statement(if_statement&&) = default;
    if_statement& operator=(if_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::IF;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Conditional for if. */
    std::unique_ptr<expression> condition;

    /** Statement to execute if condition is true. */
    std::unique_ptr<statement> branch;
};

/**
 * While statement.
 */
struct while_statement final : public statement
{
    /**
     * Construct a new while_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param condition
     *   Expression for while condition.
     *
     * @param branch
     *   Statement to execute while condition is true.
     */
    while_statement(
        token tok,
        std::unique_ptr<expression> &&condition,
        std::unique_ptr<statement> &&branch)
        : statement(statement_type::WHILE, tok),
          condition(std::move(condition)),
          branch(std::move(branch))
    { }

    // default
    ~while_statement() override = default;
    while_statement(const while_statement&) = default;
    while_statement& operator=(const while_statement&) = default;
    while_statement(while_statement&&) = default;
    while_statement& operator=(while_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::WHILE;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Condition for loop. */
    std::unique_ptr<expression> condition;

    /** Statement to execute while condition is true. */
    std::unique_ptr<statement> branch;
};

/**
 * Do while statement.
 */
struct do_while_statement final : public statement
{
    /**
     * Construct a new do_while_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param condition
     *   Expression for loop condition.
     *
     * @param branch
     *   Statement to execute while condition is true.
     */
    do_while_statement(
        token tok,
        std::unique_ptr<expression> &&condition,
        std::unique_ptr<statement> &&branch)
        : statement(statement_type::DO_WHILE, tok),
          condition(std::move(condition)),
          branch(std::move(branch))
    { }

    // default
    ~do_while_statement() override = default;
    do_while_statement(const do_while_statement&) = default;
    do_while_statement& operator=(const do_while_statement&) = default;
    do_while_statement(do_while_statement&&) = default;
    do_while_statement& operator=(do_while_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::DO_WHILE;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Condition for loop. */
    std::unique_ptr<expression> condition;

    /** Statement to execute while condition is true. */
    std::unique_ptr<statement> branch;
};

/**
 * For statement.
 */
struct for_statement final : public statement
{
    /**
     * Construct a new for_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param init_statement
     *   Statement for loop initialisation.
     *
     * @param condition
     *   Expression for loop condition.
     *
     * @param modifier
     *   Expression to execute each loop iteration.
     *
     * @param block
     *   Statement to execute while condition is true.
     */
    for_statement(
        token tok,
        std::unique_ptr<statement> &&init_statement,
        std::unique_ptr<expression> &&condition,
        std::unique_ptr<expression> &&modifier,
        std::unique_ptr<statement> &&block)
        : statement(statement_type::FOR, tok),
          init_statement(std::move(init_statement)),
          condition(std::move(condition)),
          modifier(std::move(modifier)),
          block(std::move(block))
    { }

    // default
    ~for_statement() override = default;
    for_statement(const for_statement&) = default;
    for_statement& operator=(const for_statement&) = default;
    for_statement(for_statement&&) = default;
    for_statement& operator=(for_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::FOR;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Initialisation statement for loop. */
    std::unique_ptr<statement> init_statement;

    /** Condition for loop. */
    std::unique_ptr<expression> condition;

    /** Expression to execute each loop iteration. */
    std::unique_ptr<expression> modifier;

    /** Statement to execute while condition is true. */
    std::unique_ptr<statement> block;
};

/**
 * For each statement.
 */
struct for_each_statement final : public statement
{
    /**
     * Construct new for_each_statement.
     *
     * @param iterable
     *   Expression to iterate.
     *
     * @param block
     *   Statement to execute whilst there iterator is not exhausted.
     */
    for_each_statement(
        token iterator,
        std::unique_ptr<expression> &&iterable,
        std::unique_ptr<statement> &&block)
        : statement(statement_type::FOR_EACH, iterator),
          iterable(std::move(iterable)),
          block(std::move(block))
    { }

    // default
    ~for_each_statement() override = default;
    for_each_statement(const for_each_statement&) = default;
    for_each_statement& operator=(const for_each_statement&) = default;
    for_each_statement(for_each_statement&&) = default;
    for_each_statement& operator=(for_each_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::FOR_EACH;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Expression to iterate. */
    std::unique_ptr<expression> iterable;

    /** Statement to execute whilst iterator is not exhausted. */
    std::unique_ptr<statement> block;
};

/**
 * Function statement.
 */
struct function_statement final : public statement
{
    /**
     * Construct a new function_statement.
     *
     * @param name
     *   Token for expression.
     *
     * @param parameters.
     *   Collection of tokens for the function parameters. This takes ownership
     *   of the collection.
     *
     * @param block
     *   Statement for function body.
     */
    function_statement(
        token name,
        std::vector<token> &&parameters,
        std::unique_ptr<statement> &&block)
        : statement(statement_type::FUNCTION, name),
          parameters(std::move(parameters)),
          block(std::move(block))
    { }

    // default
    ~function_statement() override = default;
    function_statement(const function_statement&) = default;
    function_statement& operator=(const function_statement&) = default;
    function_statement(function_statement&&) = default;
    function_statement& operator=(function_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::FUNCTION;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Collection parameter tokens. */
    std::vector<token> parameters;

    /** Function body. */
    std::unique_ptr<statement> block;
};

/**
 * Return statement.
 */
struct return_statement final : public statement
{
    /**
     * Construct a new return_statement.
     *
     * @param tok
     *   Token for statement.
     *
     * @param value.
     *   Return value.
     */
    return_statement(
        token tok,
        std::unique_ptr<expression> &&value)
        : statement(statement_type::RETURN, tok),
          value(std::move(value))
    { }

    ~return_statement() override = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::RETURN;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    /** Return value. */
    std::unique_ptr<expression> value;
};

/**
 * Break statement.
 */
struct break_statement final : public statement
{
    /**
     * Construct a new break_statement.
     *
     * @param tok
     *   Token for statement.
     */
    break_statement(token tok)
        : statement(statement_type::BREAK, tok)
    { }

    // default
    ~break_statement() override = default;
    break_statement(const break_statement&) = default;
    break_statement& operator=(const break_statement&) = default;
    break_statement(break_statement&&) = default;
    break_statement& operator=(break_statement&&) = default;

    /**
     * Test if a statement base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param stmnt
     *   Base pointer to an statement.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const statement *stmnt)
    {
        return stmnt->type == statement_type::BREAK;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(statement_visitor &visitor) override
    {
        visitor.visit(*this);
    }
};

}

