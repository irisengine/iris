set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/win32")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/win32_d3d12_window.h
    ${INCLUDE_ROOT}/win32_opengl_window.h
    ${INCLUDE_ROOT}/win32_window.h
    ${INCLUDE_ROOT}/win32_window_manager.h
    win32_d3d12_window.cpp
    win32_opengl_window.cpp
    win32_window.cpp
    win32_window_manager.cpp)
