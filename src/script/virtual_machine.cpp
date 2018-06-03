#include "virtual_machine.hpp"

#include <cstdint>
#include <functional>
#include <map>
#include <stack>

#include "error.hpp"
#include "object.hpp"
#include "exception.hpp"
#include "opcode.hpp"
#include "script_exception.hpp"

namespace eng
{

virtual_machine::virtual_machine(const chunk &chnk)
    : chunk_(chnk),
      pc_(nullptr),
      line_data_(nullptr),
      stack_(),
      variables_(),
      iterators_(),
      objects_(),
      call_stack_()
{
    variables_.push({ });
    pc_ = chunk_.code().data();
    line_data_ = chunk_.line_data().top().data();
}

std::map<std::string, value> virtual_machine::globals() const
{
    std::map<std::string, value> vars{ };

    // get an iterator to the root of the cactus_stack, which are the global
    // variables
    auto iter = std::cbegin(chunk_.variables());
    while((iter + 1u) != std::cend(chunk_.variables()))
    {
        ++iter;
    }

    // copy variables into map
    for(const auto &[name, index] : *iter)
    {
        vars[name] = variables_.top().at(index);
    }

    return vars;
}

void virtual_machine::run()
{
    const auto code = chunk_.code();

    // reset internal state
    pc_ = code.data();
    line_data_ = chunk_.line_data().top().data();
    stack_ = std::stack<value>{ };
    variables_.clear();
    variables_.push({ });
    objects_.clear();

    // create empty objects, this is needed as they are passed around via
    // pointers so need to exist somehwere
    for(auto i = 0u; i < chunk_.num_objects(); ++i)
    {
        objects_.emplace_back(std::make_unique<object>());
    }

    // helper lambda to pop a value from the stack
    const auto pop = [this]()
    {
        const auto value = stack_.top();
        stack_.pop();
        return value;
    };

    // helper lambda to push a value to the stack
    const auto push = [this](const eng::value &v)
    {
        stack_.emplace(v);
    };

    // helper lambda to perform an arithmetic operator to the stop two values of
    // the stack, and push the result back
    const auto arithmetic = [&pop, &push](const auto &op)
    {
        const auto stack_arg2 = pop();
        const auto stack_arg1 = pop();
        push(op(stack_arg1, stack_arg2));
    };

    // helper lambda to perform a comparison operator to the top two values of
    // the stack, and push the result back
    const auto compare = [&pop, &push](const auto &op)
    {
        const auto stack_arg2 = pop();
        const auto stack_arg1 = pop();
        push(eng::value{ op(stack_arg1, stack_arg2) });
    };

    // helper lanbda to get the next instruction
    const auto next_instruction = [this]()
    {
        const auto instruction = *pc_;
        ++pc_;
        ++line_data_;
        return instruction;
    };

    auto run = true;

    while(run)
    {
        const auto code = next_instruction();

        try
        {
            // execute the instrution
            switch(static_cast<opcode>(code))
            {
                case opcode::PUSH_CONSTANT:
                {
                    const auto arg = next_instruction();
                    push(chunk_.constants()[arg]);
                    break;
                }
                case opcode::PUSH_OBJECT:
                {
                    const auto arg = next_instruction();
                    push(value{ objects_.at(arg).get() });
                    break;
                }
                case opcode::PUSH_VARIABLE:
                {
                    auto found = false;
                    const auto arg = next_instruction();

                    // walk up the scopes looking for the variable
                    for(const auto &vars : variables_)
                    {
                        const auto find = vars.find(arg);
                        if(find != std::cend(vars))
                        {
                            push(find->second);
                            found = true;
                            break;
                        }
                    }

                    if(!found)
                    {
                        throw script_exception(error(
                            *(line_data_ - 1),
                            error_type::RUNTIME_ERROR));
                    }

                    break;
                }
                case opcode::POP_VARIABLE:
                {
                    const auto arg = next_instruction();
                    variables_.top()[arg] = pop();
                    break;
                }
                case opcode::POP_UPVALUE:
                {
                    auto found = false;
                    const auto arg = next_instruction();

                    // walk up the stack looking for the upvalue
                    for(auto &vars : variables_)
                    {
                        auto find = vars.find(arg);
                        if(find != std::cend(vars))
                        {
                            find->second = pop();
                            found = true;
                            break;
                        }
                    }

                    if(!found)
                    {
                        throw script_exception(error(
                            *(line_data_ - 1),
                            error_type::RUNTIME_ERROR));
                    }

                    break;
                }
                case opcode::POP:
                    pop();
                    break;
                case opcode::ADD:
                    arithmetic(std::plus<eng::value>());
                    break;
                case opcode::SUB:
                    arithmetic(std::minus<eng::value>());
                    break;
                case opcode::MUL:
                    arithmetic(std::multiplies<eng::value>());
                    break;
                case opcode::DIV:
                    arithmetic(std::divides<eng::value>());
                    break;
                case opcode::CMP_EQ:
                    compare(std::equal_to<eng::value>());
                    break;
                case opcode::CMP_NEQ:
                    compare(std::not_equal_to<eng::value>());
                    break;
                case opcode::CMP_LESS:
                    compare(std::less<eng::value>());
                    break;
                case opcode::CMP_LESS_EQ:
                    compare(std::less_equal<eng::value>());
                    break;
                case opcode::CMP_MORE:
                    compare(std::greater<eng::value>());
                    break;
                case opcode::CMP_MORE_EQ:
                    compare(std::greater_equal<eng::value>());
                    break;
                case opcode::JMP_FALSE:
                {
                    const auto stack_arg = pop().get<bool>();
                    const auto arg = next_instruction();

                    if(!stack_arg)
                    {
                        pc_ += static_cast<std::int8_t>(arg);
                        line_data_ += static_cast<std::int8_t>(arg);
                    }
                    break;
                }
                case opcode::JMP:
                {
                    const auto arg = next_instruction();
                    pc_ += static_cast<std::int8_t>(arg);
                    line_data_ += static_cast<std::int8_t>(arg);
                    break;
                }
                case opcode::SET_OBJECT:
                {
                    const auto key = pop();
                    const auto value = pop();
                    const auto obj = pop();

                    (*obj.get<object*>())[key] = value;
                    push(obj);

                    break;
                }
                case opcode::GET_OBJECT:
                {
                    const auto key = pop();
                    const auto obj = pop();

                    push(obj.get<object*>()->at(key));
                    break;
                }
                case opcode::START_ITER:
                {
                    const auto obj = pop();
                    iterators_.emplace(std::begin(*obj.get<object*>()));

                    break;
                }
                case opcode::ADVANCE_ITER:
                {
                    const auto arg = next_instruction();

                    const auto obj = pop();

                    if(iterators_.empty())
                    {
                        throw script_exception(error(
                            *(line_data_ - 1),
                            error_type::ITERATOR_IMBALANCE));
                    }

                    if(iterators_.top() != std::end(*obj.get<object*>()))
                    {
                        // we are not at the end of the iterator so push the
                        // current value and advance
                        push(value{ (*iterators_.top()).first });
                        ++iterators_.top();
                    }
                    else
                    {
                        // we are at the end of the iterator so jump to the end
                        // of the loop
                        iterators_.pop();
                        pc_ += static_cast<std::int8_t>(arg);
                        line_data_ += static_cast<std::int8_t>(arg);
                    }

                    break;
                }
                case opcode::CALL:
                {
                    const auto func = pop().get<function>();
                    const auto arg = next_instruction();

                    // check we have the expected number of arguments
                    if(func.num_args != arg)
                    {
                        throw script_exception(error(
                            *(line_data_ - 1),
                            error_type::INCORRECT_ARG_NUM));
                    }

                    // create a new call stack
                    call_stack_.emplace(pc_, line_data_, std::begin(variables_));

                    // jump into function
                    pc_ = func.code->data();
                    line_data_ = func.line_data->data();

                    // set varible cactus_stack to same as function
                    variables_.advance(func.variables);

                    // create new variable scope for function
                    variables_.push({ });
                    break;
                }
                case opcode::RETURN:
                {
                    // get data from top of acall stack
                    auto [new_pc, new_line_data, new_vars] = call_stack_.top();
                    call_stack_.pop();

                    // reset state back to after function call
                    pc_ = new_pc;
                    line_data_ = new_line_data;
                    variables_.advance(new_vars);
                    break;
                }
                case opcode::CLOSURE:
                {
                    auto func = pop().get<function>();

                    // store current location in variable cactus_stack
                    func.variables = std::begin(variables_);

                    push(value{ func });
                    break;
                }
                case opcode::STOP:
                    run = false;
                    break;
                default:
                    throw script_exception(error(
                        *(line_data_ - 1),
                        error_type::UNKNOWN_OPCODE));
            }
        }
        catch(const script_exception&)
        {
            // allow script exceptions to propagate
            throw;
        }
        catch(const exception &e)
        {
            // catch all engine excpetions and propegate as script esceptions
            throw script_exception(error(
                *(line_data_ - 1),
                error_type::RUNTIME_ERROR));
        }
        catch(...)
        {
            // any other exception is an internal parser error, so report it
            // as such
            throw exception("internal virtual machine error");
        }
    }

    // check stack was empty at the end, otherwise an error has occures
    if(!stack_.empty())
    {
        throw script_exception(error(
            location_data{ 0u, 0u },
            error_type::STACK_IMBALANCE));
    }
}

}
