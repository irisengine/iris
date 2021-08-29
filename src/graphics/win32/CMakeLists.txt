set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/text_factory.cpp"
    "${SOURCE_ROOT}/win32_d3d12_window.cpp"
    "${SOURCE_ROOT}/win32_opengl_window.cpp"
    "${SOURCE_ROOT}/win32_window.cpp"
    "${SOURCE_ROOT}/win32_window_manager.cpp"
    PARENT_SCOPE)
