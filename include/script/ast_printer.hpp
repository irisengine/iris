#pragma once

#include <memory>
#include <vector>

#include "expression.hpp"
#include "expression_visitor.hpp"
#include "statement.hpp"
#include "statement_visitor.hpp"

namespace eng
{

/**
 * This class is used for printing an AST to stdout.
 */
class ast_printer final : expression_visitor, statement_visitor
{
    public:

        /**
         * Construct a new ast_printer.
         *
         * @param statements.
         *   Collection of statements which form the AST to print.
         */
        ast_printer(const std::vector<std::unique_ptr<statement>> &statements);

        // default
        ~ast_printer() override = default;
        ast_printer(const ast_printer&) = default;
        ast_printer& operator=(const ast_printer&) = default;
        ast_printer(ast_printer&&) = default;
        ast_printer& operator=(ast_printer&&) = default;

    private:

        // visitor methods for expression and statement types

        void visit(literal_expression&) override;
        void visit(variable_expression&) override;
        void visit(object_expression&) override;
        void visit(assignment_expression&) override;
        void visit(binop_expression&) override;
        void visit(get_expression&) override;
        void visit(set_expression&) override;
        void visit(call_expression&) override;

        void visit(variable_statement&) override;
        void visit(expression_statement&) override;
        void visit(block_statement&) override;
        void visit(if_statement&) override;
        void visit(while_statement&) override;
        void visit(do_while_statement&) override;
        void visit(for_statement&) override;
        void visit(for_each_statement&) override;
        void visit(function_statement&) override;
        void visit(return_statement&) override;
        void visit(break_statement&) override;
};

}

