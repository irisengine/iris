SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform")

SET(PLATFORM_SRCS
    "${INCLUDE_ROOT}/file_event_type.hpp"
    "${INCLUDE_ROOT}/keyboard_event.hpp"
    "${INCLUDE_ROOT}/mouse_event.hpp"
    "${INCLUDE_ROOT}/window.hpp"
    "${SOURCE_ROOT}/event_dispatcher.cpp" "${INCLUDE_ROOT}/event_dispatcher.hpp"
)

IF(PLATFORM MATCHES "OSX")
    SET(PLATFORM_SRCS
        ${PLATFORM_SRCS}
        "${INCLUDE_ROOT}/osx/cf_ptr.hpp"
        "${SOURCE_ROOT}/osx/AppDelegate.m" "${INCLUDE_ROOT}/osx/AppDelegate.h"
        "${SOURCE_ROOT}/osx/OpenGLView.m" "${INCLUDE_ROOT}/osx/OpenGLView.h"
        "${SOURCE_ROOT}/osx/file_watcher_implementation.mm" "${INCLUDE_ROOT}/osx/file_watcher_implementation.hpp"
        "${SOURCE_ROOT}/osx/window.mm"

        PARENT_SCOPE
    )
ENDIF()

