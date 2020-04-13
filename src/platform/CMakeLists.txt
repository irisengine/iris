SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform")

IF(PLATFORM MATCHES "OSX")
    SET(PLATFORM_SRCS
        "${INCLUDE_ROOT}/osx/cf_ptr.hpp"
        "${INCLUDE_ROOT}/osx/AppDelegate.h"
        "${SOURCE_ROOT}/osx/window.mm"
    )
ENDIF()

IF(GRAPHICS_API MATCHES "OPENGL")
    SET(PLATFORM_SRCS
        ${PLATFORM_SRCS}
        "${SOURCE_ROOT}/osx/OpenGLAppDelegate.m"
        "${SOURCE_ROOT}/osx/OpenGLView.m" "${INCLUDE_ROOT}/osx/OpenGLView.h"
    )
ELSEIF(GRAPHICS_API MATCHES "METAL")
    SET(PLATFORM_SRCS
        ${PLATFORM_SRCS}
        "${SOURCE_ROOT}/osx/MetalAppDelegate.m"
        "${SOURCE_ROOT}/osx/MetalView.m" "${INCLUDE_ROOT}/osx/MetalView.h"
    )
ENDIF()

SET(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/keyboard_event.hpp"
    "${INCLUDE_ROOT}/mouse_event.hpp"
    "${INCLUDE_ROOT}/window.hpp"
    "${SOURCE_ROOT}/event_dispatcher.cpp" "${INCLUDE_ROOT}/event_dispatcher.hpp"

    PARENT_SCOPE
)

