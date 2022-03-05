set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/metal")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/compiler_strings.h
    ${INCLUDE_ROOT}/metal_buffer.h
    ${INCLUDE_ROOT}/metal_constant_buffer.h
    ${INCLUDE_ROOT}/metal_cube_map.h
    ${INCLUDE_ROOT}/metal_material.h
    ${INCLUDE_ROOT}/metal_mesh.h
    ${INCLUDE_ROOT}/metal_mesh_manager.h
    ${INCLUDE_ROOT}/metal_render_target.h
    ${INCLUDE_ROOT}/metal_renderer.h
    ${INCLUDE_ROOT}/metal_texture.h
    ${INCLUDE_ROOT}/metal_texture_manager.h
    ${INCLUDE_ROOT}/msl_shader_compiler.h
    metal_buffer.mm
    metal_constant_buffer.mm
    metal_cube_map.mm
    metal_material.mm
    metal_mesh.mm
    metal_mesh_manager.mm
    metal_render_target.mm
    metal_renderer.mm
    metal_texture.mm
    metal_texture_manager.mm
    msl_shader_compiler.cpp)
