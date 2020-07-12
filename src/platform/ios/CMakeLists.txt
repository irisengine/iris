set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/platform/ios")

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/utility.h"
    "${SOURCE_ROOT}/../macos/macos_ios_utility.mm"
    "${SOURCE_ROOT}/AppDelegate.mm" "${INCLUDE_ROOT}/AppDelegate.h"
    "${SOURCE_ROOT}/MetalView.mm" "${INCLUDE_ROOT}/MetalView.h"
    "${SOURCE_ROOT}/MetalViewController.mm" "${INCLUDE_ROOT}/MetalViewController.h"
    "${SOURCE_ROOT}/resource_loader.mm"
    "${SOURCE_ROOT}/start.mm"
    "${SOURCE_ROOT}/window.mm"

    PARENT_SCOPE
)

