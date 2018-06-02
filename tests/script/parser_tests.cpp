#include <gtest/gtest.h>

#include "cast.hpp"
#include "expression.hpp"
#include "expression_type.hpp"
#include "lexer.hpp"
#include "panic_exception.hpp"
#include "parser.hpp"
#include "statement.hpp"
#include "statement_type.hpp"

namespace
{

template<class T, class S>
const T* test_and_get(const std::unique_ptr<S> &ptr)
{
    const auto *casted = eng::dyn_cast<T>(ptr);
    EXPECT_NE(nullptr, casted);
    return casted;
}

}

TEST(parser_tests, empty_program)
{
    const auto source = R"src()src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_TRUE( actual_statements.empty());
}

TEST(parser_tests, ignore_empty_statements)
{
    const auto source = R"src(;;;;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_TRUE( actual_statements.empty());
}

TEST(parser_tests, single_number_statement)
{
    const auto source = R"src(0.1;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *lit_expr = test_and_get<eng::literal_expression>(expr_stmnt->expr);

    ASSERT_EQ(eng::token_type::NUMBER, lit_expr->tok.type);
    ASSERT_EQ(0.1f, lit_expr->tok.val.get<float>());
}

TEST(parser_tests, single_string_statement)
{
    const auto source = R"src('hello';)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *lit_expr = test_and_get<eng::literal_expression>(expr_stmnt->expr);

    ASSERT_EQ(eng::token_type::STRING, lit_expr->tok.type);
    ASSERT_EQ("hello", lit_expr->tok.val.get<std::string>());
}

TEST(parser_tests, single_true_statement)
{
    const auto source = R"src(true;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *lit_expr = test_and_get<eng::literal_expression>(expr_stmnt->expr);

    ASSERT_EQ(eng::token_type::TRUE, lit_expr->tok.type);
    ASSERT_EQ(true, lit_expr->tok.val.get<bool>());
}

TEST(parser_tests, single_false_statement)
{
    const auto source = R"src(false;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *lit_expr = test_and_get<eng::literal_expression>(expr_stmnt->expr);

    ASSERT_EQ(eng::token_type::FALSE, lit_expr->tok.type);
    ASSERT_EQ(false, lit_expr->tok.val.get<bool>());
}

TEST(parser_tests, single_number_access_statement)
{
    const auto source = R"src(a[0];)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto *get_expr = test_and_get<eng::get_expression>(expr_stmnt->expr);
    const auto *var_expr = test_and_get<eng::variable_expression>(get_expr->object);

    ASSERT_EQ("a", var_expr->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(get_expr->member);

    ASSERT_EQ(eng::token_type::NUMBER, lit_expr->tok.type);
    ASSERT_EQ(0.0f, lit_expr->tok.val.get<float>());
}

TEST(parser_tests, single_string_access_statement)
{
    const auto source = R"src(a['hi'];)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto *get_expr = test_and_get<eng::get_expression>(expr_stmnt->expr);
    const auto *var_expr = test_and_get<eng::variable_expression>(get_expr->object);

    ASSERT_EQ("a", var_expr->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(get_expr->member);

    ASSERT_EQ(eng::token_type::STRING, lit_expr->tok.type);
    ASSERT_EQ("hi", lit_expr->tok.val.get<std::string>());
}

TEST(parser_tests, single_bool_access_statement)
{
    const auto source = R"src(a[false];)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto *get_expr = test_and_get<eng::get_expression>(expr_stmnt->expr);
    const auto *var_expr = test_and_get<eng::variable_expression>(get_expr->object);

    ASSERT_EQ("a", var_expr->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(get_expr->member);

    ASSERT_EQ(eng::token_type::FALSE, lit_expr->tok.type);
    ASSERT_EQ(false, lit_expr->tok.val.get<bool>());
}

TEST(parser_tests, declare_number_var)
{
    const auto source = R"src(var a = 0.1;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *var_stmnt = static_cast<eng::variable_statement*>(actual_statements[0].get());


    ASSERT_EQ("a", var_stmnt->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(var_stmnt->value);

    ASSERT_EQ(0.1f, lit_expr->tok.val.get<float>());
}

TEST(parser_tests, declare_string_var)
{
    const auto source = R"src(var a = 'hello';)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *var_stmnt = static_cast<eng::variable_statement*>(actual_statements[0].get());


    ASSERT_EQ("a", var_stmnt->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(var_stmnt->value);

    ASSERT_EQ("hello", lit_expr->tok.val.get<std::string>());
}

TEST(parser_tests, declare_bool_var)
{
    const auto source = R"src(var a = true;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *var_stmnt = static_cast<eng::variable_statement*>(actual_statements[0].get());


    ASSERT_EQ("a", var_stmnt->tok.lexeme);

    const auto *lit_expr = test_and_get<eng::literal_expression>(var_stmnt->value);

    ASSERT_EQ(true, lit_expr->tok.val.get<bool>());
}

TEST(parser_tests, declare_empty_object_var)
{
    const auto source = R"src(var a = {};)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *var_stmnt = static_cast<eng::variable_statement*>(actual_statements[0].get());

    ASSERT_EQ("a", var_stmnt->tok.lexeme);

    const auto *obj_expr = static_cast<eng::object_expression*>(var_stmnt->value.get());

    ASSERT_TRUE(obj_expr->members.empty());
}

TEST(parser_tests, single_binop_plus_statement)
{
    const auto source = R"src(1 + 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::PLUS, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_minus_statement)
{
    const auto source = R"src(1 - 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::MINUS, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_multiply_statement)
{
    const auto source = R"src(1 * 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::STAR, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_divide_statement)
{
    const auto source = R"src(1 / 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::SLASH, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_equals_statement)
{
    const auto source = R"src(1 == 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::EQUAL_EQUAL, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_not_equals_statement)
{
    const auto source = R"src(1 != 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::BANG_EQUAL, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_less_statement)
{
    const auto source = R"src(1 < 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::LESS, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_less_equal_statement)
{
    const auto source = R"src(1 <= 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::LESS_EQUAL, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_more_statement)
{
    const auto source = R"src(1 > 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::MORE, binop_expr->tok.type);
}

TEST(parser_tests, single_binop_more_equal_statement)
{
    const auto source = R"src(1 >= 2;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr = test_and_get<eng::binop_expression>(expr_stmnt->expr);

    const auto *lit_expr1 = test_and_get<eng::literal_expression>(binop_expr->left);
    ASSERT_EQ(1.0f, lit_expr1->tok.val.get<float>());

    const auto *lit_expr2 = test_and_get<eng::literal_expression>(binop_expr->right);
    ASSERT_EQ(2.0f, lit_expr2->tok.val.get<float>());

    ASSERT_EQ(eng::token_type::MORE_EQUAL, binop_expr->tok.type);
}

TEST(parser_tests, multi_binop_precedence)
{
    const auto source = R"src(1 + 2 == 3 * 4 > 5;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr1 = test_and_get<eng::binop_expression>(expr_stmnt->expr);
    ASSERT_EQ(eng::token_type::MORE, binop_expr1->tok.type);

    const auto *lit_exp1 = test_and_get<eng::literal_expression>(binop_expr1->right);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp1->tok.type);
    ASSERT_EQ(5.0f, lit_exp1->tok.val.get<float>());

    const auto *binop_expr2 = test_and_get<eng::binop_expression>(binop_expr1->left);
    ASSERT_EQ(eng::token_type::EQUAL_EQUAL, binop_expr2->tok.type);

    const auto *binop_expr3 = test_and_get<eng::binop_expression>(binop_expr2->left);
    ASSERT_EQ(eng::token_type::PLUS, binop_expr3->tok.type);

    const auto *lit_exp2 = test_and_get<eng::literal_expression>(binop_expr3->left);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp2->tok.type);
    ASSERT_EQ(1.0f, lit_exp2->tok.val.get<float>());

    const auto *lit_exp3 = test_and_get<eng::literal_expression>(binop_expr3->right);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp3->tok.type);
    ASSERT_EQ(2.0f, lit_exp3->tok.val.get<float>());

    const auto *binop_expr4 = test_and_get<eng::binop_expression>(binop_expr2->right);
    ASSERT_EQ(eng::token_type::STAR, binop_expr4->tok.type);

    const auto *lit_exp4 = test_and_get<eng::literal_expression>(binop_expr4->left);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp4->tok.type);
    ASSERT_EQ(3.0f, lit_exp4->tok.val.get<float>());

    const auto *lit_exp5 = test_and_get<eng::literal_expression>(binop_expr4->right);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp5->tok.type);
    ASSERT_EQ(4.0f, lit_exp5->tok.val.get<float>());
}

TEST(parser_tests, multi_binop_brackets)
{
    const auto source = R"src((1 + 2) * 3;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();

    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);

    const auto *binop_expr1 = test_and_get<eng::binop_expression>(expr_stmnt->expr);
    ASSERT_EQ(eng::token_type::STAR, binop_expr1->tok.type);

    const auto *lit_exp1 = test_and_get<eng::literal_expression>(binop_expr1->right);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp1->tok.type);
    ASSERT_EQ(3.0f, lit_exp1->tok.val.get<float>());

    const auto *binop_expr2 = test_and_get<eng::binop_expression>(binop_expr1->left);
    ASSERT_EQ(eng::token_type::PLUS, binop_expr2->tok.type);

    const auto *lit_exp2 = test_and_get<eng::literal_expression>(binop_expr2->left);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp2->tok.type);
    ASSERT_EQ(1.0f, lit_exp2->tok.val.get<float>());

    const auto *lit_exp3 = test_and_get<eng::literal_expression>(binop_expr2->right);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp3->tok.type);
    ASSERT_EQ(2.0f, lit_exp3->tok.val.get<float>());
}

TEST(parser_tests, block)
{
    const auto source = R"src({0.1;})src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *block_stmnt = test_and_get<eng::block_statement>(actual_statements[0u]);
    ASSERT_EQ(1u, block_stmnt->statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(block_stmnt->statements[0]);

    const auto *lit_exp = test_and_get<eng::literal_expression>(expr_stmnt->expr);
    ASSERT_EQ(eng::token_type::NUMBER, lit_exp->tok.type);
    ASSERT_EQ(0.1f, lit_exp->tok.val.get<float>());
}

TEST(parser_tests, simple_if)
{
    const auto source = R"src(if(true){0.1;})src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *if_stmnt = test_and_get<eng::if_statement>(actual_statements[0]);

    test_and_get<eng::literal_expression>(if_stmnt->condition);
    test_and_get<eng::block_statement>(if_stmnt->branch);
}

TEST(parser_tests, simple_while)
{
    const auto source = R"src(while(true){0.1;})src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *while_stmnt = test_and_get<eng::while_statement>(actual_statements[0]);

    test_and_get<eng::literal_expression>(while_stmnt->condition);
    test_and_get<eng::block_statement>(while_stmnt->branch);
}

TEST(parser_tests, simple_do_while)
{
    const auto source = R"src(do{0.1;}while(true))src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *do_while_stmnt = test_and_get<eng::do_while_statement>(actual_statements[0]);

    test_and_get<eng::literal_expression>(do_while_stmnt->condition);
    test_and_get<eng::block_statement>(do_while_stmnt->branch);
}

TEST(parser_tests, simple_for)
{
    const auto source = R"src(for(var i=0;i<5;i=1){0.1;})src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *for_stmnt = test_and_get<eng::for_statement>(actual_statements[0]);

    test_and_get<eng::variable_statement>(for_stmnt->init_statement);
    test_and_get<eng::binop_expression>(for_stmnt->condition);
    test_and_get<eng::assignment_expression>(for_stmnt->modifier);
    test_and_get<eng::block_statement>(for_stmnt->block);
}

TEST(parser_tests, simple_for_each)
{
    const auto source = R"src(foreach(var i in a){0.1;})src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *for_each_stmnt = test_and_get<eng::for_each_statement>(actual_statements[0]);
    ASSERT_EQ(eng::token_type::IDENTIFIER, for_each_stmnt->tok.type);
    ASSERT_EQ("i", for_each_stmnt->tok.lexeme);

    test_and_get<eng::variable_expression>(for_each_stmnt->iterable);
    test_and_get<eng::block_statement>(for_each_stmnt->block);
}

TEST(parser_tests, simple_break)
{
    const auto source = R"src(break;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    test_and_get<eng::break_statement>(actual_statements[0]);
}

TEST(parser_tests, simple_call)
{
    const auto source = R"src(a();)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto *call_expr = test_and_get<eng::call_expression>(expr_stmnt->expr);

    test_and_get<eng::variable_expression>(call_expr->callee);
}

TEST(parser_tests, call_with_args)
{
    const auto source = R"src(a(true, 'hello');)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto call_expr = test_and_get<eng::call_expression>(expr_stmnt->expr);

    test_and_get<eng::variable_expression>(call_expr->callee);

    ASSERT_EQ(2u, call_expr->arguments.size());

    test_and_get<eng::literal_expression>(call_expr->arguments[0u]);
    test_and_get<eng::literal_expression>(call_expr->arguments[1u]);
}

TEST(parser_tests, multi_call)
{
    const auto source = R"src(a()();)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    const auto call_expr1 = test_and_get<eng::call_expression>(expr_stmnt->expr);
    ASSERT_TRUE(call_expr1->arguments.empty());

    const auto call_expr2 = test_and_get<eng::call_expression>(call_expr1->callee);

    test_and_get<eng::variable_expression>(call_expr2->callee);
}

TEST(parser_tests, simple_return)
{
    const auto source = R"src(return 1;)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *ret_stmnt = test_and_get<eng::return_statement>(actual_statements[0]);
    test_and_get<eng::literal_expression>(ret_stmnt->value);
}

TEST(parser_tests, simple_function)
{
    const auto source = R"src(function foo() { return 1; })src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *func_stmnt = test_and_get<eng::function_statement>(actual_statements[0]);
    ASSERT_EQ("foo", func_stmnt->tok.lexeme);
    ASSERT_TRUE(func_stmnt->parameters.empty());

    test_and_get<eng::block_statement>(func_stmnt->block);
}

TEST(parser_tests, function_with_args)
{
    const auto source = R"src(function foo(var a, var b) { return 1; })src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *func_stmnt = test_and_get<eng::function_statement>(actual_statements[0]);
    ASSERT_EQ("foo", func_stmnt->tok.lexeme);
    ASSERT_EQ(2u, func_stmnt->parameters.size());

    ASSERT_EQ("a", func_stmnt->parameters[0u].lexeme);
    ASSERT_EQ("b", func_stmnt->parameters[1u].lexeme);

    test_and_get<eng::block_statement>(func_stmnt->block);
}

TEST(parser_tests, function_implicit_return)
{
    const auto source = R"src(function foo() { })src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *func_stmnt = test_and_get<eng::function_statement>(actual_statements[0]);
    const auto *block_stmnt = test_and_get<eng::block_statement>(func_stmnt->block);

    ASSERT_EQ(1u, block_stmnt->statements.size());

    test_and_get<eng::return_statement>(block_stmnt->statements[0]);
}

TEST(parser_tests, return_object)
{
    const auto source = R"src(foo(x)[0];)src";

    eng::lexer l{ source };
    eng::parser p{ l.tokens() };

    const auto actual_statements = p.yield();
    ASSERT_EQ(1u, actual_statements.size());

    const auto *expr_stmnt = test_and_get<eng::expression_statement>(actual_statements[0]);
    test_and_get<eng::get_expression>(expr_stmnt->expr);
}

