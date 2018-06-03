#pragma once

#include <memory>
#include <vector>
#include <stack>

#include "chunk.hpp"
#include "expression.hpp"
#include "expression_visitor.hpp"
#include "statement.hpp"
#include "statement_visitor.hpp"

namespace eng
{

/**
 * This class consumes a collection of statements and produces a chunk, a
 * collection of raw bytes and supporting data, which will allow the virtual
 * machine to execute the statements.
 *
 * Makes use of the visitor pattern to walk the statements and form the chunk.
 */
class compiler final : expression_visitor, statement_visitor
{
    public:

        /**
         * Construct a new compiler object.
         *
         * @param statements
         *   Collection of statements to compile.
         */
        compiler(const std::vector<std::unique_ptr<statement>> &statements);

        // default
        ~compiler() override = default;
        compiler(const compiler&) = default;
        compiler& operator=(const compiler&) = default;
        compiler(compiler&&) = default;
        compiler& operator=(compiler&&) = default;

        /**
         * Get the compiled chunk.
         *
         * @returns
         *   Compiled chunk.
         */
        chunk get_chunk() const;

    private:

        // visitor methods for expression and statement types

        void visit(literal_expression&) override;
        void visit(variable_expression&) override;
        void visit(assignment_expression&) override;
        void visit(object_expression&) override;
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

        /** Chunk contatining compiled program. */
        chunk chunk_;

        /** Flag indicating additional clean up is required. */
        bool clean_;

        /** Flag indiciating we are using upvalues. */
        bool upvalue_;

        /** Stack of jumps that will need to be patched. */
        std::stack<std::stack<std::uint32_t>> jumps_;

};

}



