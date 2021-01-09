set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/platform")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("macos")
elseif(IRIS_PLATFORM MATCHES "IOS")
  add_subdirectory("ios")
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("windows")
endif()

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/keyboard_event.h"
    "${INCLUDE_ROOT}/mouse_event.h"
    "${INCLUDE_ROOT}/quit_event.h"
    "${INCLUDE_ROOT}/resource_loader.h"
    "${INCLUDE_ROOT}/start.h"
    "${INCLUDE_ROOT}/static_buffer.h"
    "${INCLUDE_ROOT}/thread.h"
    "${INCLUDE_ROOT}/touch_event.h"
    "${INCLUDE_ROOT}/window.h"
    "${SOURCE_ROOT}/event.cpp"
    "${INCLUDE_ROOT}/event.h"
    PARENT_SCOPE)
