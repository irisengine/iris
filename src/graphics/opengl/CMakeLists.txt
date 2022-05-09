set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/opengl")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/compiler_strings.h
    ${INCLUDE_ROOT}/glsl_shader_compiler.h
    ${INCLUDE_ROOT}/opengl.h
    ${INCLUDE_ROOT}/opengl_buffer.h
    ${INCLUDE_ROOT}/opengl_cube_map.h
    ${INCLUDE_ROOT}/opengl_frame_buffer.h
    ${INCLUDE_ROOT}/opengl_material.h
    ${INCLUDE_ROOT}/opengl_mesh.h
    ${INCLUDE_ROOT}/opengl_mesh_manager.h
    ${INCLUDE_ROOT}/opengl_render_target.h
    ${INCLUDE_ROOT}/opengl_renderer.h
    ${INCLUDE_ROOT}/opengl_sampler.h
    ${INCLUDE_ROOT}/opengl_shader.h
    ${INCLUDE_ROOT}/opengl_texture.h
    ${INCLUDE_ROOT}/opengl_texture_manager.h
    ${INCLUDE_ROOT}/opengl_uniform.h
    glsl_shader_compiler.cpp
    opengl.cpp
    opengl_cube_map.cpp
    opengl_frame_buffer.cpp
    opengl_material.cpp
    opengl_mesh.cpp
    opengl_mesh_manager.cpp
    opengl_render_target.cpp
    opengl_renderer.cpp
    opengl_sampler.cpp
    opengl_shader.cpp
    opengl_texture.cpp
    opengl_texture_manager.cpp
    opengl_uniform.cpp)

if(IRIS_PLATFORM MATCHES "WIN32")
  target_sources(iris PRIVATE ${INCLUDE_ROOT}/opengl_windows.h)
endif()
