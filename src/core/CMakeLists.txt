set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/core")

set(CORE_SRCS
    "${SOURCE_ROOT}/camera.cpp" "${INCLUDE_ROOT}/camera.hpp"
    "${SOURCE_ROOT}/exception.cpp" "${INCLUDE_ROOT}/exception.hpp"
    "${SOURCE_ROOT}/matrix3.cpp" "${INCLUDE_ROOT}/matrix3.hpp"
    "${SOURCE_ROOT}/matrix4.cpp" "${INCLUDE_ROOT}/matrix4.hpp"
    "${SOURCE_ROOT}/quaternion.cpp" "${INCLUDE_ROOT}/quaternion.hpp"
    "${SOURCE_ROOT}/vector3.cpp" "${INCLUDE_ROOT}/vector3.hpp"

    PARENT_SCOPE
)

