set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/core")

set(CORE_SRCS
    "${SOURCE_ROOT}/camera.cpp" "${INCLUDE_ROOT}/camera.h"
    "${SOURCE_ROOT}/exception.cpp" "${INCLUDE_ROOT}/exception.h"
    "${SOURCE_ROOT}/matrix4.cpp" "${INCLUDE_ROOT}/matrix4.h"
    "${SOURCE_ROOT}/quaternion.cpp" "${INCLUDE_ROOT}/quaternion.h"
    "${SOURCE_ROOT}/root.cpp" "${INCLUDE_ROOT}/root.h"
    "${SOURCE_ROOT}/vector3.cpp" "${INCLUDE_ROOT}/vector3.h"

    PARENT_SCOPE
)
