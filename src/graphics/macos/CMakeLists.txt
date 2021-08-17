set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/macos_window.mm"
    "${SOURCE_ROOT}/macos_window_manager.cpp"
    "${SOURCE_ROOT}/metal_app_delegate.m"
    "${SOURCE_ROOT}/metal_view.m"
    "${SOURCE_ROOT}/opengl_app_delegate.m"
    "${SOURCE_ROOT}/opengl_view.m"
    "${SOURCE_ROOT}/text_factory.mm"
    PARENT_SCOPE)
