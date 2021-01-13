set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core")

set(CORE_SRCS
    "${INCLUDE_ROOT}/auto_release.h"
    "${INCLUDE_ROOT}/camera_type.h"
    "${SOURCE_ROOT}/camera.cpp"
    "${INCLUDE_ROOT}/camera.h"
    "${SOURCE_ROOT}/exception.cpp"
    "${INCLUDE_ROOT}/exception.h"
    "${SOURCE_ROOT}/looper.cpp"
    "${INCLUDE_ROOT}/looper.h"
    "${SOURCE_ROOT}/matrix4.cpp"
    "${INCLUDE_ROOT}/matrix4.h"
    "${SOURCE_ROOT}/quaternion.cpp"
    "${INCLUDE_ROOT}/quaternion.h"
    "${SOURCE_ROOT}/random.cpp"
    "${INCLUDE_ROOT}/random.h"
    "${SOURCE_ROOT}/transform.cpp"
    "${INCLUDE_ROOT}/transform.h"
    "${SOURCE_ROOT}/utils.cpp"
    "${INCLUDE_ROOT}/utils.h"
    "${SOURCE_ROOT}/vector3.cpp"
    "${INCLUDE_ROOT}/vector3.h"
    PARENT_SCOPE)
