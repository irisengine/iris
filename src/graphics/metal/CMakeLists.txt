set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/metal")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/metal_buffer.h
    ${INCLUDE_ROOT}/metal_constant_buffer.h
    ${INCLUDE_ROOT}/metal_cube_map.h
    ${INCLUDE_ROOT}/metal_material.h
    ${INCLUDE_ROOT}/metal_material_manager.h
    ${INCLUDE_ROOT}/metal_mesh.h
    ${INCLUDE_ROOT}/metal_mesh_manager.h
    ${INCLUDE_ROOT}/metal_render_target.h
    ${INCLUDE_ROOT}/metal_render_target_manager.h
    ${INCLUDE_ROOT}/metal_renderer.h
    ${INCLUDE_ROOT}/metal_sampler.h
    ${INCLUDE_ROOT}/metal_texture.h
    ${INCLUDE_ROOT}/metal_texture_manager.h
    metal_buffer.mm
    metal_constant_buffer.mm
    metal_cube_map.mm
    metal_material.mm
    metal_material_manager.mm
    metal_mesh.mm
    metal_mesh_manager.mm
    metal_render_target.mm
    metal_render_target_manager.mm
    metal_renderer.mm
    metal_sampler.mm
    metal_texture.mm
    metal_texture_manager.mm)
