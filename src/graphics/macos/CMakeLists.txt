set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/macos")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/macos_window.h
    ${INCLUDE_ROOT}/macos_window_manager.h
    ${INCLUDE_ROOT}/metal_app_delegate.h
    ${INCLUDE_ROOT}/metal_view.h
    macos_window.mm
    macos_window_manager.cpp
    metal_app_delegate.m
    metal_view.m
)
