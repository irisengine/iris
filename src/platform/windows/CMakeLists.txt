set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/platform/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/platform/macos")

set(PLATFORM_SRCS
    ${PLATFORM_SRCS}
    "${DEFAULT_ROOT}/start.cpp"
    "${DEFAULT_ROOT}/resource_loader.cpp"
    "${SOURCE_ROOT}/window.cpp"

    PARENT_SCOPE
)

