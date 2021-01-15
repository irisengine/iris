set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("macos")
elseif(IRIS_PLATFORM MATCHES "IOS")
  add_subdirectory("ios")
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("windows")
endif()

set(CORE_SRCS
    ${CORE_SRCS}
    "${INCLUDE_ROOT}/auto_release.h"
    "${INCLUDE_ROOT}/camera.h"
    "${INCLUDE_ROOT}/camera_type.h"
    "${INCLUDE_ROOT}/colour.h"
    "${INCLUDE_ROOT}/exception.h"
    "${INCLUDE_ROOT}/looper.h"
    "${INCLUDE_ROOT}/matrix4.h"
    "${INCLUDE_ROOT}/quaternion.h"
    "${INCLUDE_ROOT}/random.h"
    "${INCLUDE_ROOT}/transform.h"
    "${INCLUDE_ROOT}/utils.h"
    "${INCLUDE_ROOT}/vector3.h"
    "${SOURCE_ROOT}/camera.cpp"
    "${SOURCE_ROOT}/colour.cpp"
    "${SOURCE_ROOT}/exception.cpp"
    "${SOURCE_ROOT}/looper.cpp"
    "${SOURCE_ROOT}/matrix4.cpp"
    "${SOURCE_ROOT}/quaternion.cpp"
    "${SOURCE_ROOT}/random.cpp"
    "${SOURCE_ROOT}/transform.cpp"
    "${SOURCE_ROOT}/utils.cpp"
    "${SOURCE_ROOT}/vector3.cpp"
    "${INCLUDE_ROOT}/resource_loader.h"
    "${INCLUDE_ROOT}/start.h"
    "${INCLUDE_ROOT}/static_buffer.h"
    "${INCLUDE_ROOT}/thread.h"
    "${INCLUDE_ROOT}/window.h"
    PARENT_SCOPE)
