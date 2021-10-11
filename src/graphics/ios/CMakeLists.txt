set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/ios")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/app_delegate.h
    ${INCLUDE_ROOT}/ios_window.h
    ${INCLUDE_ROOT}/ios_window_manager.h
    ${INCLUDE_ROOT}/metal_view.h
    ${INCLUDE_ROOT}/metal_view_controller.h
    app_delegate.mm
    ios_window.mm
    ios_window_manager.cpp
    metal_view.mm
    metal_view_controller.mm)
