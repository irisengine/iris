set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform")

if(PLATFORM MATCHES "OSX")
    add_subdirectory("osx")
endif()


set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/keyboard_event.hpp"
    "${INCLUDE_ROOT}/mouse_event.hpp"
    "${INCLUDE_ROOT}/window.hpp"
    "${SOURCE_ROOT}/event.cpp" "${INCLUDE_ROOT}/event.hpp"

    PARENT_SCOPE
)

