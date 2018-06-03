#include "compiler.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include "cactus_stack.hpp"
#include "cast.hpp"
#include "chunk.hpp"
#include "error.hpp"
#include "expression.hpp"
#include "expression_visitor.hpp"
#include "opcode.hpp"
#include "panic_exception.hpp"
#include "script_exception.hpp"
#include "statement.hpp"
#include "statement_type.hpp"
#include "statement_visitor.hpp"

namespace
{

/**
 * Helper function to walk up a cactus_stack of variables searching for a named
 * variable. Will throw a panic_exception if variable is not found.
 *
 * @param tok
 *   Token containing variable name to search for.
 *
 * @param variables
 *   cactus_stack of variables to search.
 *
 * @returns
 *   Unique index of variable.
 */
std::size_t variable_index(
    const eng::token &tok,
    const eng::cactus_stack<std::map<std::string, std::uint32_t>> &variables)
{
    auto index = 0u;
    auto found = false;

    // start at current scope and walk up the cactus_stack, towards the root
    for(const auto &scope : variables)
    {
        // search for variable
        const auto find = scope.find(tok.lexeme);
        if(find != std::cend(scope))
        {
            // we found it, so store its unique index and stop
            index = find->second;
            found = true;
            break;
        }
    }

    if(!found)
    {
        throw eng::panic_exception(eng::error_type::UNDECLARED_VARIABLE, tok);
    }

    return index;
}

}

namespace eng
{

compiler::compiler(const std::vector<std::unique_ptr<statement>> &statements)
    : chunk_(),
      clean_(true),
      upvalue_(false),
      jumps_()
{
    std::vector<error> errors{ };

    // we first run through all statements looking for function declarations so
    // we can add variable names for them
    // this allows the script to call functions that have not yet been defined
    // without it casuing a compiler error
    for(const auto &statement : statements)
    {
        if(const auto *func = eng::dyn_cast<eng::function_statement>(statement) ; func != nullptr)
        {
            chunk_.add_variable(func->tok.lexeme);
        }
    }

    // compile all statements
    for(const auto &statement : statements)
    {
        // here we will try and compile each statement
        // if a panic_exception is thrown then we will 'synchronise' and
        // continue compiling, this allows us a chance to report as many errors
        // as possible
        // to synchronise we will discard the current statement and continue,
        // this is a best guess effort and can lead to cascading errors, at
        // least the first error should be correct!
        try
        {
            // try and compile statement
            statement->accept(*this);
        }
        catch(const panic_exception &e)
        {
            // caught a panic exception so store the error and synchronise

            const auto token = e.bad_token();
            const auto location = token.location;
            errors.emplace_back(
                location_data{ location.line, location.offset + token.lexeme.length() },
                e.type());

            continue;
        }
        catch(const exception &e)
        {
            // allow regular engine exceptions to propagate
            throw;
        }
        catch(...)
        {
            // any other exception is an internal compiler error, so report it
            // as such
            throw exception("internal compiler error");
        }
    }

    // if we encountered any errors then propagate them as an exception
    if(!errors.empty())
    {
        throw script_exception(errors);
    }

    // last instruction is alwasy STOP, this allows the virtual machine to
    // gracefully end execution
    chunk_.add_instruction(opcode::STOP);
}

void compiler::visit(literal_expression &expr)
{
    const auto index = chunk_.add_constant(expr.tok.val);

    // compile to PUSH_CONSTANT + index of added constant
    chunk_.add_instruction(opcode::PUSH_CONSTANT, expr.tok);
    chunk_.add_raw_instruction(index, expr.tok);
}

void compiler::visit(variable_expression &expr)
{
    const auto index = variable_index(expr.tok, chunk_.variables());

    // compile to PUSH_VARIABLE + index of added variable
    chunk_.add_instruction(opcode::PUSH_VARIABLE, expr.tok);
    chunk_.add_raw_instruction(index, expr.tok);
}

void compiler::visit(assignment_expression &expr)
{
    // an assignment will not need cleaning up as we will pop the value at the
    // end
    clean_ = false;

    // compile the expression value
    expr.value->accept(*this);

    // get index of variable name
    const auto index = variable_index(expr.tok, chunk_.variables());

    // pop the value as either a variable or an upvalue, depending in the
    // current state of our statement compilation
    upvalue_
        ? chunk_.add_instruction(opcode::POP_UPVALUE, expr.tok)
        : chunk_.add_instruction(opcode::POP_VARIABLE, expr.tok);

    // always add the index of the variable after the instruction
    chunk_.add_raw_instruction(index, expr.tok);
}

void compiler::visit(object_expression &expr)
{
    // compile to PUSH_OBJECT + index of added object
    const auto index = chunk_.add_object();
    chunk_.add_instruction(opcode::PUSH_OBJECT, expr.tok);
    chunk_.add_raw_instruction(index, expr.tok);

    // compile each member and set it to the object
    for(const auto &[key, value] : expr.members)
    {
        value->accept(*this);
        key->accept(*this);

        chunk_.add_instruction(opcode::SET_OBJECT, expr.tok);
    }
}

void compiler::visit(binop_expression &expr)
{
    // compile both sides of the binary expression
    expr.left->accept(*this);
    expr.right->accept(*this);

    // handle operator
    switch(expr.tok.type)
    {
        case token_type::PLUS:
            chunk_.add_instruction(opcode::ADD, expr.tok);
            break;
        case token_type::MINUS:
            chunk_.add_instruction(opcode::SUB, expr.tok);
            break;
        case token_type::STAR:
            chunk_.add_instruction(opcode::MUL, expr.tok);
            break;
        case token_type::SLASH:
            chunk_.add_instruction(opcode::DIV, expr.tok);
            break;
        case token_type::EQUAL_EQUAL:
            chunk_.add_instruction(opcode::CMP_EQ, expr.tok);
            break;
        case token_type::BANG_EQUAL:
            chunk_.add_instruction(opcode::CMP_NEQ, expr.tok);
            break;
        case token_type::LESS:
            chunk_.add_instruction(opcode::CMP_LESS, expr.tok);
            break;
        case token_type::LESS_EQUAL:
            chunk_.add_instruction(opcode::CMP_LESS_EQ, expr.tok);
            break;
        case token_type::MORE:
            chunk_.add_instruction(opcode::CMP_MORE, expr.tok);
            break;
        case token_type::MORE_EQUAL:
            chunk_.add_instruction(opcode::CMP_MORE_EQ, expr.tok);
            break;
        default:
            throw panic_exception(eng::error_type::UNEXPECTED_SYMBOL, expr.tok);
    }
}

void compiler::visit(get_expression &expr)
{
    // compile object and member
    expr.object->accept(*this);
    expr.member->accept(*this);

    chunk_.add_instruction(opcode::GET_OBJECT, expr.tok);
}

void compiler::visit(set_expression &expr)
{
    // compile object, value and member
    expr.object->accept(*this);
    expr.value->accept(*this);
    expr.member->accept(*this);

    chunk_.add_instruction(opcode::SET_OBJECT, expr.tok);
}

void compiler::visit(call_expression &expr)
{
    // compile each argument in reverse order so they are on the stack in the
    // correct order
    std::for_each(
        std::crbegin(expr.arguments),
        std::crend(expr.arguments),
        [this](const auto &expr) { expr->accept(*this); });

    // compile callee expression
    expr.callee->accept(*this);

    // compile to CALL + number of args
    chunk_.add_instruction(opcode::CALL, expr.tok);
    chunk_.add_raw_instruction(expr.arguments.size(), expr.tok);
}

void compiler::visit(variable_statement &stmnt)
{
    // a variable statement will not need cleaning up as we will pop the value at
    // the end
    clean_ = false;

    // compile value
    stmnt.value->accept(*this);

    const auto index = chunk_.add_variable(stmnt.tok.lexeme);

    // compile to POP + variable index
    chunk_.add_instruction(opcode::POP_VARIABLE, stmnt.tok);
    chunk_.add_raw_instruction(index, stmnt.tok);
}

void compiler::visit(expression_statement &stmnt)
{
    // expression statemented *may* need additional clean up as they are not
    // popped at the end
    clean_ = true;

    // compile statement
    stmnt.expr->accept(*this);

    // if during compiling the statement we did not clean up e.g. a literal
    // then add a POP
    if(clean_)
    {
        chunk_.add_instruction(opcode::POP, stmnt.tok);
    }
}

void compiler::visit(block_statement &stmnt)
{
    // add new scope for block
    chunk_.push_scope();

    // compile all statements in the block
    for(const auto &s : stmnt.statements)
    {
        s->accept(*this);
    }

    // return to original scope
    chunk_.pop_scope();
}

void compiler::visit(if_statement &stmnt)
{
    // compile condition statement
    stmnt.condition->accept(*this);

    // jump if false to end of statement, we use a placeholder landing as we
    // will have to patch at the end
    chunk_.add_instruction(opcode::JMP_FALSE, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // compile the if statement block
    stmnt.branch->accept(*this);

    // patch the jump
    const auto landing_index = chunk_.last_instruction();
    chunk_.patch_jump(jump_index, landing_index);
}

void compiler::visit(while_statement &stmnt)
{
    // get the index of the start of the loop, so we can jump back
    const auto loop_index = chunk_.last_instruction();

    // compile condition statement
    stmnt.condition->accept(*this);

    // jump if false to end of statement, we use a placeholder landing as we
    // will have to patch at the end
    chunk_.add_instruction(opcode::JMP_FALSE, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // add a new scope to jumps container, so we can patch up any break
    // statements at the end
    jumps_.push({});

    // compile the while statement block
    stmnt.branch->accept(*this);

    // add jump back to the start of the loop
    chunk_.add_instruction(opcode::JMP, stmnt.tok);
    chunk_.add_raw_instruction(loop_index - chunk_.code().size(), stmnt.tok);

    // patch the jump
    const auto landing_index = chunk_.last_instruction();
    chunk_.patch_jump(jump_index, landing_index);

    // patch any break statements that were compiled in the block
    while(!jumps_.top().empty())
    {
        chunk_.patch_jump(jumps_.top().top(), landing_index);
        jumps_.top().pop();
    }
    jumps_.pop();
}

void compiler::visit(do_while_statement &stmnt)
{
    // get the index of the start of the loop, so we can jump back
    const auto loop_index = chunk_.last_instruction();

    // add a new scope to jumps container, so we can patch up any break
    // statements at the end
    jumps_.push({});

    // compile the do while statement block
    stmnt.branch->accept(*this);

    // compile the condition statement
    stmnt.condition->accept(*this);

    // jump if false to end of statement, we use a placeholder landing as we
    // will have to patch at the end
    chunk_.add_instruction(opcode::JMP_FALSE, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // add jump back to the start of the loop
    chunk_.add_instruction(opcode::JMP, stmnt.tok);
    chunk_.add_raw_instruction(loop_index - chunk_.code().size(), stmnt.tok);

    // patch the jump
    const auto landing_index = chunk_.last_instruction();
    chunk_.patch_jump(jump_index, landing_index);

    // patch any break statements that were compiled in the block
    while(!jumps_.top().empty())
    {
        chunk_.patch_jump(jumps_.top().top(), landing_index);
        jumps_.top().pop();
    }
    jumps_.pop();
}

void compiler::visit(for_statement &stmnt)
{
    // push a new scope at the start to include the variable declared in the
    // initialisation statement
    chunk_.push_scope();

    // add a new scope to jumps container, so we can patch up any break
    // statements at the end
    jumps_.push({});

    // compile initialisation statement
    stmnt.init_statement->accept(*this);

    // get the index so we can jump back to after the initialisation
    // statement
    const auto loop_index = chunk_.last_instruction();

    // compile the condition statement
    stmnt.condition->accept(*this);

    // jump if false to end of statement, we use a placeholder landing as we
    // will have to patch at the end
    chunk_.add_instruction(opcode::JMP_FALSE, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // compile the for statement block
    stmnt.block->accept(*this);

    // compile the modifier statement
    stmnt.modifier->accept(*this);

    // add jump back to the start of the loop
    chunk_.add_instruction(opcode::JMP, stmnt.tok);
    chunk_.add_raw_instruction(loop_index - chunk_.code().size(), stmnt.tok);

    // patch the jump
    const auto landing_index = chunk_.last_instruction();
    chunk_.patch_jump(jump_index, landing_index);

    chunk_.pop_scope();

    // patch any break statements that were compiled in the block
    while(!jumps_.top().empty())
    {
        chunk_.patch_jump(jumps_.top().top(), landing_index);
        jumps_.top().pop();
    }
    jumps_.pop();
}

void compiler::visit(for_each_statement &stmnt)
{
    // push a new scope at the start to include the iterator varaible
    chunk_.push_scope();

    // add a new scope to jumps container, so we can patch up any break
    // statements at the end
    jumps_.push({});

    // create a placeholder constant for the iterator
    const auto const_index = chunk_.add_constant(value{ 0.0f });
    chunk_.add_instruction(opcode::PUSH_CONSTANT, stmnt.tok);
    chunk_.add_raw_instruction(const_index, stmnt.tok);

    // create the iterator variable
    const auto index = chunk_.add_variable(stmnt.tok.lexeme);
    chunk_.add_instruction(opcode::POP_VARIABLE, stmnt.tok);
    chunk_.add_raw_instruction(index, stmnt.tok);

    // compile the iterable and start the iteration
    stmnt.iterable->accept(*this);
    chunk_.add_instruction(opcode::START_ITER, stmnt.tok);

    // get the index so we can jump back to the start of the loop
    const auto loop_index = chunk_.last_instruction();

    // compile the iterable again, this ensures it is always evaluates at the
    // start of the loop
    stmnt.iterable->accept(*this);

    // advance the iterator to the next value
    // this will dereference the iterator, push that value to the stack and
    // advance it
    // if the iterator is at the end then it will jump, which is why we put
    // a placeholder jump distance at the end
    chunk_.add_instruction(opcode::ADVANCE_ITER, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // pop the current iterator value into the iterator variable
    chunk_.add_instruction(opcode::POP_VARIABLE, stmnt.tok);
    chunk_.add_raw_instruction(index, stmnt.tok);

    // compile the for each statement block
    stmnt.block->accept(*this);

    // add jump back to the start of the loop
    chunk_.add_instruction(opcode::JMP, stmnt.tok);
    chunk_.add_raw_instruction(loop_index - chunk_.code().size(), stmnt.tok);

    // patch the jump
    const auto landing_index = chunk_.last_instruction();
    chunk_.patch_jump(jump_index, landing_index);

    chunk_.pop_scope();

    // patch any break statements that were compiled in the block
    while(!jumps_.top().empty())
    {
        chunk_.patch_jump(jumps_.top().top(), landing_index);
        jumps_.top().pop();
    }
    jumps_.pop();
}

void compiler::visit(function_statement &stmnt)
{
    // functions are fist class objects so can be stored as values
    const auto var_index = chunk_.add_variable(stmnt.tok.lexeme);

    // push new variable and function scope
    chunk_.push_scope();
    chunk_.push_function();

    // pop each parameter into a variable
    for(const auto &param : stmnt.parameters)
    {
        const auto param_index = chunk_.add_variable(param.lexeme);
        chunk_.add_instruction(opcode::POP_VARIABLE, stmnt.tok);
        chunk_.add_raw_instruction(param_index, stmnt.tok);
    }

    // compile the function block, note that we set the upvalue_ flag to true
    // for this
    // this means that when compiling the block any POP_VARIABLE instructions
    // will become POP_UPVALUE, allowing the virtual machine to search for
    // values in the associated closure
    upvalue_ = true;
    stmnt.block->accept(*this);
    upvalue_ = false;

    // add function as a constant
    const auto *function_addr = &chunk_.functions().top();
    const auto *line_data = &chunk_.line_data().top();
    chunk_.pop_function();
    const auto const_index = chunk_.add_constant(
        value{ function{ function_addr, line_data, stmnt.parameters.size() }});

    // compile to
    //   push function constant
    //   create closure
    //   pop function into variable
    chunk_.add_instruction(opcode::PUSH_CONSTANT, stmnt.tok);
    chunk_.add_raw_instruction(const_index, stmnt.tok);
    chunk_.add_instruction(opcode::CLOSURE, stmnt.tok);
    chunk_.add_instruction(opcode::POP_VARIABLE, stmnt.tok);
    chunk_.add_raw_instruction(var_index, stmnt.tok);

    chunk_.pop_scope();
}

void compiler::visit(return_statement &stmnt)
{
    // compile return value
    stmnt.value->accept(*this);

    chunk_.add_instruction(opcode::RETURN, stmnt.tok);
}

void compiler::visit(break_statement &stmnt)
{
    if(jumps_.empty())
    {
        throw panic_exception(error_type::BREAK_SCOPE, stmnt.tok);
    }

    // insert jump instruction with a placeholder jump distance
    chunk_.add_instruction(opcode::JMP, stmnt.tok);
    const auto jump_index = chunk_.add_raw_instruction(0xff, stmnt.tok);

    // add jump instruction index to stack, this allows the current loop
    // statement we are in to patch us to the end of the associated loop
    jumps_.top().emplace(jump_index);
}

chunk compiler::get_chunk() const
{
    return chunk_;
}

}

