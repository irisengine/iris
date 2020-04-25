set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform/macos")

if(IRIS_GRAPHICS_API MATCHES "OPENGL")
    set(PLATFORM_SRCS
        "${SOURCE_ROOT}/OpenGLAppDelegate.m"
        "${SOURCE_ROOT}/OpenGLView.m" "${INCLUDE_ROOT}/OpenGLView.h"
    )
elseif(IRIS_GRAPHICS_API MATCHES "METAL")
    set(PLATFORM_SRCS
        "${SOURCE_ROOT}/MetalAppDelegate.m"
        "${SOURCE_ROOT}/MetalView.m" "${INCLUDE_ROOT}/MetalView.h"
    )
endif()

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/cf_ptr.h"
    "${INCLUDE_ROOT}/AppDelegate.h"
    "${SOURCE_ROOT}/window.mm"

    PARENT_SCOPE
)

