#include "ast_printer.hpp"

#include <iostream>

#include "expression.hpp"
#include "expression_visitor.hpp"
#include "statement.hpp"
#include "statement_visitor.hpp"

namespace eng
{

ast_printer::ast_printer(
    const std::vector<std::unique_ptr<statement>> &statements)
{
    for(const auto &statement : statements)
    {
        statement->accept(*this);
    }
}

void ast_printer::visit(literal_expression &exp)
{
    std::cout << exp.tok.val;
}

void ast_printer::visit(variable_expression &exp)
{
    std::cout << exp.tok.lexeme;
}

void ast_printer::visit(assignment_expression &exp)
{
    std::cout << exp.tok.lexeme << " = ";
    exp.value->accept(*this);
}

void ast_printer::visit(object_expression &exp)
{
    std::cout << "{" << std::endl;

    for(auto &[key, value] : exp.members)
    {
        key->accept(*this);
        std::cout << " : ";
        value->accept(*this);
        std::cout << "," << std::endl;
    }

    std::cout << "}" << std::endl;
}

void ast_printer::visit(binop_expression &exp)
{
    std::cout << "(";
    exp.left->accept(*this);

    switch(exp.tok.type)
    {
        case token_type::PLUS:
            std::cout << " + ";
            break;
        case token_type::MINUS:
            std::cout << " - ";
            break;
        case token_type::STAR:
            std::cout << " * ";
            break;
        case token_type::SLASH:
            std::cout << " / ";
            break;
        case token_type::EQUAL_EQUAL:
            std::cout << " == ";
            break;
        case token_type::BANG_EQUAL:
            std::cout << " != ";
            break;
        case token_type::LESS:
            std::cout << " < ";
            break;
        case token_type::LESS_EQUAL:
            std::cout << " <= ";
            break;
        case token_type::MORE:
            std::cout << " > ";
            break;
        case token_type::MORE_EQUAL:
            std::cout << " >= ";
            break;
        default:
            std::cout << " ? ";
            break;
    }

    exp.right->accept(*this);
    std::cout << ")";
}

void ast_printer::visit(get_expression &exp)
{
    exp.object->accept(*this);
    std::cout << "[";
    exp.member->accept(*this);
    std::cout << "]";
}

void ast_printer::visit(set_expression &exp)
{
    exp.object->accept(*this);
    std::cout << "[";
    exp.member->accept(*this);
    std::cout << "] = ";
    exp.value->accept(*this);
}

void ast_printer::visit(call_expression &exp)
{
    exp.callee->accept(*this);
    std::cout << "(";
    for(const auto &arg : exp.arguments)
    {
        arg->accept(*this);
    }
    std::cout << ")";
}

void ast_printer::visit(variable_statement &stmnt)
{
    std::cout << "var " << stmnt.tok.lexeme << " = ";
    stmnt.value->accept(*this);
    std::cout << ";" << std::endl;
}

void ast_printer::visit(expression_statement &stmnt)
{
    stmnt.expr->accept(*this);
    std::cout << ";" << std::endl;
}

void ast_printer::visit(block_statement &stmnt)
{
    std::cout << "{" << std::endl;

    for(const auto &s : stmnt.statements)
    {
        s->accept(*this);
    }

    std::cout << "}" << std::endl;
}

void ast_printer::visit(if_statement &stmnt)
{
    std::cout << "if(";
    stmnt.condition->accept(*this);
    std::cout << ")" << std::endl;

    stmnt.branch->accept(*this);
}

void ast_printer::visit(while_statement &stmnt)
{
    std::cout << "while(";
    stmnt.condition->accept(*this);
    std::cout << ")" << std::endl;

    stmnt.branch->accept(*this);
}

void ast_printer::visit(do_while_statement &stmnt)
{
    std::cout << "do" << std::endl;
    stmnt.branch->accept(*this);

    std::cout << "while(";
    stmnt.condition->accept(*this);
    std::cout << ")" << std::endl;
}

void ast_printer::visit(for_statement &stmnt)
{
    std::cout << "for(";
    stmnt.init_statement->accept(*this);
    stmnt.condition->accept(*this);
    std::cout << "; ";
    stmnt.modifier->accept(*this);
    std::cout << ")" << std::endl;

    stmnt.block->accept(*this);
}

void ast_printer::visit(for_each_statement &stmnt)
{
    std::cout << "for(var " << stmnt.tok.lexeme << " in ";
    stmnt.iterable->accept(*this);
    std::cout << ")" << std::endl;

    stmnt.block->accept(*this);
}

void ast_printer::visit(function_statement &stmnt)
{
    std::cout << "function " << stmnt.tok.lexeme << "(";
    for(const auto &p : stmnt.parameters)
    {
        std::cout << p.lexeme << ",";
    }
    std::cout << ")" << std::endl;

    stmnt.block->accept(*this);
}

void ast_printer::visit(return_statement &stmnt)
{
    std::cout << "return ";
    stmnt.value->accept(*this);
    std::cout << ";" << std::endl;
}

void ast_printer::visit(break_statement&)
{
    std::cout << "break;" << std::endl;
}

}

