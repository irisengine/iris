set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/d3d12")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/d3d12_buffer.cpp"
    "${SOURCE_ROOT}/d3d12_mesh.cpp"
    PARENT_SCOPE)
