#pragma once

#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace eng
{

/**
 * Enumeration of opcodes supported by the virtual machine.
 */
enum class opcode : std::uint8_t
{
    PUSH_CONSTANT,
    PUSH_OBJECT,
    PUSH_VARIABLE,
    POP_VARIABLE,
    POP_UPVALUE,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    CMP_EQ,
    CMP_NEQ,
    CMP_LESS,
    CMP_LESS_EQ,
    CMP_MORE,
    CMP_MORE_EQ,
    JMP_FALSE,
    JMP,
    SET_OBJECT,
    GET_OBJECT,
    START_ITER,
    ADVANCE_ITER,
    CALL,
    RETURN,
    CLOSURE,
    STOP
};

/**
 * Assemble an opcode into a raw byte.
 *
 * @param op
 *   Opcode to assemble.
 *
 * @returns
 *   Opcode as a raw byte.
 */
inline std::uint8_t assemble(const opcode op)
{
    return static_cast<std::uint8_t>(op);
}

/**
 * Disassemble a collection of bytes.
 *
 *
 * @param code
 *   Collection of raw bytes.
 *
 * @return
 *   Dissasembled code as a string.
 */
inline std::string disassemble(const std::vector<std::uint8_t> &code)
{
    std::stringstream strm{ };

    auto iter = std::cbegin(code);

    // keep looping through all bytes
    while(iter != std::cend(code))
    {
        // fetch the next byte (if it exists) as this may be used as an
        // argument
        const auto arg = (iter + 1) != std::cend(code)
            ? *(iter + 1)
            : 0u;

        switch(static_cast<opcode>(*iter))
        {
            case opcode::PUSH_CONSTANT:
                strm << "pushc " << arg;
                ++iter;
                break;
            case opcode::PUSH_OBJECT:
                strm << "pusho " << arg;
                ++iter;
                break;
            case opcode::PUSH_VARIABLE:
                strm << "pushv " << arg;
                ++iter;
                break;
            case opcode::POP_VARIABLE:
                strm << "popv " << arg;
                ++iter;
                break;
            case opcode::POP_UPVALUE:
                strm << "popu " << arg;
                ++iter;
                ++iter;
                break;
            case opcode::POP:
                strm << "pop";
                break;
            case opcode::ADD:
                strm << "add";
                break;
            case opcode::SUB:
                strm << "sub";
                break;
            case opcode::MUL:
                strm << "mul";
                break;
            case opcode::DIV:
                strm << "div";
                break;
            case opcode::CMP_EQ:
                strm << "cmpeq " << arg;
                ++iter;
                break;
            case opcode::CMP_NEQ:
                strm << "cmpneq " << arg;
                ++iter;
                break;
            case opcode::CMP_LESS:
                strm << "cmpl " << arg;
                ++iter;
                break;
            case opcode::CMP_LESS_EQ:
                strm << "cmple " << arg;
                ++iter;
                break;
            case opcode::CMP_MORE:
                strm << "cmpm " << arg;
                ++iter;
                break;
            case opcode::CMP_MORE_EQ:
                strm << "cmpme " << arg;
                ++iter;
                break;
            case opcode::JMP_FALSE:
                strm << "jmpfls " << +static_cast<std::int8_t>(arg);
                ++iter;
                break;
            case opcode::JMP:
                strm << "jmp " << +static_cast<std::int8_t>(arg);
                ++iter;
                break;
            case opcode::SET_OBJECT:
                strm << "setobj";
                break;
            case opcode::GET_OBJECT:
                strm << "getobj";
                break;
            case opcode::START_ITER:
                strm << "startiter";
                break;
            case opcode::ADVANCE_ITER:
                strm << "advanceiter " << +static_cast<std::int8_t>(arg);
                ++iter;
                break;
            case opcode::STOP:
                strm << "stop";
                break;
            case opcode::CALL:
                strm << "call " << +static_cast<std::int8_t>(arg);
                ++iter;
                break;
            case opcode::RETURN:
                strm << "ret";
                break;
            case opcode::CLOSURE:
                strm << "closure";
                break;
            default:
                strm << "UNKNOWN: " << +*iter;
        }

        ++iter;

        if(iter != std::cend(code))
        {
            strm << std::endl;
        }
    }

    return strm.str();
}

}

