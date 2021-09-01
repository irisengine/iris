set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/win32")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/win32_d3d12_window.cpp
    ${INCLUDE_ROOT}/win32_opengl_window.cpp
    ${INCLUDE_ROOT}/win32_window.cpp
    ${INCLUDE_ROOT}/win32_window_manager.cpp
    text_factory.cpp
    win32_d3d12_window.cpp
    win32_opengl_window.cpp
    win32_window.cpp
    win32_window_manager.cpp
    PARENT_SCOPE)
