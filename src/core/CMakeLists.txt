SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/core")

SET(CORE_SRCS
    "${SOURCE_ROOT}/camera.cpp" "${INCLUDE_ROOT}/camera.hpp"
    "${SOURCE_ROOT}/matrix3.cpp" "${INCLUDE_ROOT}/matrix3.hpp"
    "${SOURCE_ROOT}/matrix4.cpp" "${INCLUDE_ROOT}/matrix4.hpp"
    "${SOURCE_ROOT}/quaternion.cpp" "${INCLUDE_ROOT}/quaternion.hpp"
    "${SOURCE_ROOT}/vector3.cpp" "${INCLUDE_ROOT}/vector3.hpp"

    PARENT_SCOPE
)

