set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/render_system.mm"
    "${SOURCE_ROOT}/metal_buffer.mm"
    "${SOURCE_ROOT}/metal_material.mm"
    "${SOURCE_ROOT}/metal_mesh.mm"
    "${SOURCE_ROOT}/metal_mesh_manager.mm"
    "${SOURCE_ROOT}/metal_render_target.mm"
    "${SOURCE_ROOT}/metal_texture.mm"
    "${SOURCE_ROOT}/msl_shader_compiler.cpp"
    PARENT_SCOPE)
