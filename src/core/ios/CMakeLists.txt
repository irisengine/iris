set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/ios")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/AppDelegate.h
    ${INCLUDE_ROOT}/MetalView.h
    ${INCLUDE_ROOT}/MetalViewController.h
    ${INCLUDE_ROOT}/utility.h
    ../macos/macos_ios_utility.mm
    AppDelegate.mm
    MetalView.mm
    MetalViewController.mm
    resource_loader.mm
    start.mm
    window.mm)
