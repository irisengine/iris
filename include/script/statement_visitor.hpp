#pragma once

namespace eng
{

// forward declarations
struct variable_statement;
struct expression_statement;
struct block_statement;
struct if_statement;
struct while_statement;
struct do_while_statement;
struct for_statement;
struct for_each_statement;
struct function_statement;
struct return_statement;
struct break_statement;

/**
 * A class defining virtual methods for visiting each type of statement.
 *
 * Note that whilst this class does have a virtual destructor derived classes
 * should use private inheritance to prevent polluting the classes public API.
 */
class statement_visitor
{
    public:

        // default
        statement_visitor() = default;
        virtual ~statement_visitor() = default;
        statement_visitor(const statement_visitor&) = default;
        statement_visitor& operator=(const statement_visitor&) = default;
        statement_visitor(statement_visitor&&) = default;
        statement_visitor& operator=(statement_visitor&&) = default;

        // virtual visit methods
        virtual void visit(variable_statement&) = 0;
        virtual void visit(expression_statement&) = 0;
        virtual void visit(block_statement&) = 0;
        virtual void visit(if_statement&) = 0;
        virtual void visit(while_statement&) = 0;
        virtual void visit(do_while_statement&) = 0;
        virtual void visit(for_statement&) = 0;
        virtual void visit(for_each_statement&) = 0;
        virtual void visit(function_statement&) = 0;
        virtual void visit(return_statement&) = 0;
        virtual void visit(break_statement&) = 0;
};

}

