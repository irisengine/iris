set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform")

if(PLATFORM MATCHES "OSX")
    set(PLATFORM_SRCS
        "${INCLUDE_ROOT}/osx/cf_ptr.hpp"
        "${INCLUDE_ROOT}/osx/AppDelegate.h"
        "${SOURCE_ROOT}/osx/window.mm"
    )
endif()

if(GRAPHICS_API MATCHES "OPENGL")
    set(PLATFORM_SRCS
        ${PLATFORM_SRCS}
        "${SOURCE_ROOT}/osx/OpenGLAppDelegate.m"
        "${SOURCE_ROOT}/osx/OpenGLView.m" "${INCLUDE_ROOT}/osx/OpenGLView.h"
    )
elseif(GRAPHICS_API MATCHES "METAL")
    set(PLATFORM_SRCS
        ${PLATFORM_SRCS}
        "${SOURCE_ROOT}/osx/MetalAppDelegate.m"
        "${SOURCE_ROOT}/osx/MetalView.m" "${INCLUDE_ROOT}/osx/MetalView.h"
    )
endif()

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/keyboard_event.hpp"
    "${INCLUDE_ROOT}/mouse_event.hpp"
    "${INCLUDE_ROOT}/window.hpp"
    "${SOURCE_ROOT}/event.cpp" "${INCLUDE_ROOT}/event.hpp"

    PARENT_SCOPE
)

