set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/linux")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/linux_window.h
    ${INCLUDE_ROOT}/linux_window_manager.h
    ${INCLUDE_ROOT}/scoped_error_handler.h
    linux_window.cpp
    linux_window_manager.cpp)
