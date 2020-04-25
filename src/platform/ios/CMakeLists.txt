set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform/ios")

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/utility.h"
    "${SOURCE_ROOT}/../macos/macos_ios_utility.mm"

    PARENT_SCOPE
)

