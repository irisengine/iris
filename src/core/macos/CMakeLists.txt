set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/macos")

if(IRIS_GRAPHICS_API MATCHES "OPENGL")
  set(CORE_SRCS "${SOURCE_ROOT}/OpenGLAppDelegate.m"
                "${SOURCE_ROOT}/OpenGLView.m" "${INCLUDE_ROOT}/OpenGLView.h")
elseif(IRIS_GRAPHICS_API MATCHES "METAL")
  set(CORE_SRCS "${SOURCE_ROOT}/MetalAppDelegate.m"
                "${SOURCE_ROOT}/MetalView.m" "${INCLUDE_ROOT}/MetalView.h")
endif()

set(CORE_SRCS
    ${CORE_SRCS}
    "${DEFAULT_ROOT}/resource_loader.cpp"
    "${DEFAULT_ROOT}/start.cpp"
    "${INCLUDE_ROOT}/AppDelegate.h"
    "${INCLUDE_ROOT}/macos_ios_utility.h"
    "${INCLUDE_ROOT}/utility.h"
    "${SOURCE_ROOT}/macos_ios_utility.mm"
    "${SOURCE_ROOT}/static_buffer.cpp"
    "${SOURCE_ROOT}/thread.cpp"
    "${SOURCE_ROOT}/window.mm"
    PARENT_SCOPE)
