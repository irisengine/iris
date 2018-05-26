#pragma once

namespace eng
{

// forward declarations
struct literal_expression;
struct variable_expression;
struct assignment_expression;
struct object_expression;
struct binop_expression;
struct get_expression;
struct set_expression;
struct call_expression;

/**
 * A class defining virtual methods for visiting each type of expression.
 *
 * Note that whilst this class does have a virtual destructor derived classes
 * should use private inheritance to prevent polluting the classes public API.
 */
class expression_visitor
{
    public:

        // default
        expression_visitor() = default;
        virtual ~expression_visitor() = default;
        expression_visitor(const expression_visitor&) = default;
        expression_visitor& operator=(const expression_visitor&) = default;
        expression_visitor(expression_visitor&&) = default;
        expression_visitor& operator=(expression_visitor&&) = default;

        // virtual visit methods
        virtual void visit(literal_expression&) = 0;
        virtual void visit(variable_expression&) = 0;
        virtual void visit(assignment_expression&) = 0;
        virtual void visit(object_expression&) = 0;
        virtual void visit(binop_expression&) = 0;
        virtual void visit(get_expression&) = 0;
        virtual void visit(set_expression&) = 0;
        virtual void visit(call_expression&) = 0;
};

}

