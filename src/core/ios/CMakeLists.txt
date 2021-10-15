set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/core/ios")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/utility.h
    ../macos/macos_ios_utility.mm
    resource_loader.mm
    start.mm)
