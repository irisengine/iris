#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "expression_type.hpp"
#include "expression_visitor.hpp"
#include "token.hpp"

namespace eng
{

/**
 * Base struct for an expression object. This stores a token as all expressions
 * come from some token. Where an expression is formed of several tokens the
 * 'core' token should be passed to the base constructor and the rest stored in
 * the derived struct. If an error occurs later in the compilation process then
 * the token stored in the base will be used for generating error information.
 */
struct expression
{
    /**
     * Construct a base expression.
     *
     * @param type
     *   The type of expression.
     *
     * @param tok
     *   The token that forms this expression.
     */
    expression(expression_type type, token tok)
        : type(type),
          tok(tok)
    { }

    // default
    virtual ~expression() = default;
    expression(const expression&) = default;
    expression& operator=(const expression&) = default;
    expression(expression&&) = default;
    expression& operator=(expression&&) = default;

    /**
     * Accept a visitor.
     */
    virtual void accept(expression_visitor&) = 0;

    /** Type of expression. */
    expression_type type;

    /** Core token of expression. */
    token tok;
};

/**
 * Expression for a literal, this could be a string, number, bool etc.
 */
struct literal_expression final : expression
{
    /**
     * Construct a literal expression.
     *
     * @param tok
     *   Token for expression.
     */
    literal_expression(token tok)
        : expression(expression_type::LITERAL, tok)
    { }

    // default
    ~literal_expression() override = default;
    literal_expression(const literal_expression&) = default;
    literal_expression& operator=(const literal_expression&) = default;
    literal_expression(literal_expression&&) = default;
    literal_expression& operator=(literal_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::LITERAL;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };
};

/**
 * Expression for a variable.
 */
struct variable_expression final : expression
{
    /**
     * Construct a new variable_expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param expr
     *   The expression to assign.
     */
    variable_expression(token tok)
        : expression(expression_type::VARIABLE, tok)
    { }

    // default
    ~variable_expression() override = default;
    variable_expression(const variable_expression&) = default;
    variable_expression& operator=(const variable_expression&) = default;
    variable_expression(variable_expression&&) = default;
    variable_expression& operator=(variable_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::VARIABLE;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };
};

/**
 * Expression for assignment.
 */
struct assignment_expression final : expression
{
    /**
     * Construct a new assignment_expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param value
     *   The expression to assign.
     */
    assignment_expression(token tok, std::unique_ptr<expression> value)
        : expression(expression_type::ASSIGNMENT, tok),
          value(std::move(value))
    { }

    // default
    ~assignment_expression() override = default;
    assignment_expression(const assignment_expression&) = default;
    assignment_expression& operator=(const assignment_expression&) = default;
    assignment_expression(assignment_expression&&) = default;
    assignment_expression& operator=(assignment_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::ASSIGNMENT;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };

    /** The expression to assign. */
    std::unique_ptr<expression> value;
};

/**
 * Expression for an object.
 */
struct object_expression final : expression
{
    /**
     * Construct a new object expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param members
     *   Collection of members for the object. This takes ownership of the
     *   collection.
     */
    object_expression(
        token tok,
        std::vector<std::pair<std::unique_ptr<expression>, std::unique_ptr<expression>>> &&members)
        : expression(expression_type::OBJECT, tok),
          members(std::move(members))
    { }

    // default
    ~object_expression() override = default;
    object_expression(const object_expression&) = default;
    object_expression& operator=(const object_expression&) = default;
    object_expression(object_expression&&) = default;
    object_expression& operator=(object_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::OBJECT;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    }

    std::vector<std::pair<std::unique_ptr<expression>, std::unique_ptr<expression>>> members;
};

/**
 * Expression for a binary operator.
 */
struct binop_expression final : expression
{
    /**
     * Construct a new binop_expression.
     *
     * @param left
     *   Expression on left side of operator.
     *
     * @param tok
     *   Token for expression.
     *
     * @param right
     *   Expression on right side of operator.
     */
    binop_expression(
        std::unique_ptr<expression> left,
        token tok,
        std::unique_ptr<expression> right)
        : expression(expression_type::BINOP, tok),
          left(std::move(left)),
          right(std::move(right))
    { }

    // default
    ~binop_expression() override = default;
    binop_expression(const binop_expression&) = default;
    binop_expression& operator=(const binop_expression&) = default;
    binop_expression(binop_expression&&) = default;
    binop_expression& operator=(binop_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::BINOP;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };

    /** Expression on left side of operator. */
    std::unique_ptr<expression> left;

    /** Expression on right side of operator. */
    std::unique_ptr<expression> right;
};

/**
 * Expression for getting an element from an object.
 */
struct get_expression final : expression
{
    /**
     * Construct a new get_expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param object
     *   Expression for object to get.
     *
     * @param member
     *   Expression for member of object to get.
     */
    get_expression(
        token tok,
        std::unique_ptr<expression> object,
        std::unique_ptr<expression> member)
        : expression(expression_type::GET, tok),
          object(std::move(object)),
          member(std::move(member))
    { }

    // default
    ~get_expression() override = default;
    get_expression(const get_expression&) = default;
    get_expression& operator=(const get_expression&) = default;
    get_expression(get_expression&&) = default;
    get_expression& operator=(get_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::GET;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };

    /** Expression for object to get. */
    std::unique_ptr<expression> object;

    /** Expression for member of object to get. */
    std::unique_ptr<expression> member;
};

/**
 * Expression for setting an element from an object.
 */
struct set_expression final : expression
{
    /**
     * Construct a new set_expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param object
     *   Expression for object to set.
     *
     * @param member
     *   Expression for member of object to set.
     *
     * @param value
     *   Expression of value to assign.
     */
    set_expression(
        token tok,
        std::unique_ptr<expression> object,
        std::unique_ptr<expression> member,
        std::unique_ptr<expression> value)
        : expression(expression_type::SET, tok),
          object(std::move(object)),
          member(std::move(member)),
          value(std::move(value))
    { }

    // default
    ~set_expression() override = default;
    set_expression(const set_expression&) = default;
    set_expression& operator=(const set_expression&) = default;
    set_expression(set_expression&&) = default;
    set_expression& operator=(set_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::SET;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };

    /** Expression for object to set. */
    std::unique_ptr<expression> object;

    /** Expression for member of object to set. */
    std::unique_ptr<expression> member;

    /** Expression for value to assign. */
    std::unique_ptr<expression> value;
};

/**
 * Expression for calling a function.
 */
struct call_expression final : expression
{
    /**
     * Construct a new call_expression.
     *
     * @param tok
     *   Token for expression.
     *
     * @param callee
     *   Expression for callee of function.
     *
     * @param arguments
     *   Collection of expressions for arguments. Takes ownership of collection.
     */
    call_expression(
        token tok,
        std::unique_ptr<expression> callee,
        std::vector<std::unique_ptr<expression>> &&arguments)
        : expression(expression_type::CALL, tok),
          callee(std::move(callee)),
          arguments(std::move(arguments))
    { }

    // default
    ~call_expression() override = default;
    call_expression(const call_expression&) = default;
    call_expression& operator=(const call_expression&) = default;
    call_expression(call_expression&&) = default;
    call_expression& operator=(call_expression&&) = default;

    /**
     * Test if an expression base pointer is actually this derived class. This
     * forms part of the custom RTTI system.
     *
     * @param expr
     *   Base pointer to an expression.
     *
     * @returns
     *   True if base pointer is actually this derived class, false otherwise.
     */
    static bool is(const expression *expr)
    {
        return expr->type == expression_type::CALL;
    }

    /**
     * Accept a visitor.
     *
     * @param visitor
     *   The visitor to accept.
     */
    void accept(expression_visitor &visitor) override
    {
        visitor.visit(*this);
    };

    /** Expression for callee of function. */
    std::unique_ptr<expression> callee;

    /** Collection of expressions for arguments. Takes ownership of collection. */
    std::vector<std::unique_ptr<expression>> arguments;
};

}

