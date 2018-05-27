SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/script")

SET(SCRIPT_SRCS
    "${INCLUDE_ROOT}/cactus_stack.hpp"
    "${INCLUDE_ROOT}/cast.hpp"
    "${INCLUDE_ROOT}/error_type.hpp"
    "${INCLUDE_ROOT}/expression.hpp"
    "${INCLUDE_ROOT}/expression_type.hpp"
    "${INCLUDE_ROOT}/expression_visitor.hpp"
    "${INCLUDE_ROOT}/function.hpp"
    "${INCLUDE_ROOT}/location_data.hpp"
    "${INCLUDE_ROOT}/object.hpp"
    "${INCLUDE_ROOT}/statement.hpp"
    "${INCLUDE_ROOT}/statement_type.hpp"
    "${INCLUDE_ROOT}/statement_visitor.hpp"
    "${INCLUDE_ROOT}/token.hpp"
    "${INCLUDE_ROOT}/token_type.hpp"
    "${INCLUDE_ROOT}/value.hpp"
    "${SOURCE_ROOT}/error.cpp" "${INCLUDE_ROOT}/error.hpp"
    "${SOURCE_ROOT}/lexer.cpp" "${INCLUDE_ROOT}/lexer.hpp"
    "${SOURCE_ROOT}/script_exception.cpp" "${INCLUDE_ROOT}/script_exception.hpp"

    PARENT_SCOPE
)

