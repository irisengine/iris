set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/graphics/osx")

set(PLATFORM_SRCS
    "${INCLUDE_ROOT}/cf_ptr.hpp"
    "${SOURCE_ROOT}/font.mm"

    PARENT_SCOPE
)


