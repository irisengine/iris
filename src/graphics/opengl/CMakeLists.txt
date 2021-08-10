set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/opengl")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${INCLUDE_ROOT}/shader_type.h"
    "${SOURCE_ROOT}/opengl.cpp"
    "${INCLUDE_ROOT}/opengl.h"
    "${SOURCE_ROOT}/render_system.cpp"
    "${INCLUDE_ROOT}/opengl_shader.h"
    "${SOURCE_ROOT}/glsl_shader_compiler.cpp"
    "${SOURCE_ROOT}/opengl_buffer.cpp"
    "${SOURCE_ROOT}/opengl_material.cpp"
    "${SOURCE_ROOT}/opengl_mesh.cpp"
    "${SOURCE_ROOT}/opengl_render_target.cpp"
    "${SOURCE_ROOT}/opengl_shader.cpp"
    "${SOURCE_ROOT}/opengl_texture.cpp"
    PARENT_SCOPE)
