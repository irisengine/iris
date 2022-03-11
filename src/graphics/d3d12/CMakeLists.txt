set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/d3d12")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/d3d12_buffer.h
    ${INCLUDE_ROOT}/d3d12_constant_buffer.h
    ${INCLUDE_ROOT}/d3d12_constant_buffer_pool.h
    ${INCLUDE_ROOT}/d3d12_context.h
    ${INCLUDE_ROOT}/d3d12_cpu_descriptor_handle_allocator.h
    ${INCLUDE_ROOT}/d3d12_cube_map.h
    ${INCLUDE_ROOT}/d3d12_descriptor_handle.h
    ${INCLUDE_ROOT}/d3d12_descriptor_manager.h
    ${INCLUDE_ROOT}/d3d12_gpu_descriptor_handle_allocator.h
    ${INCLUDE_ROOT}/d3d12_material.h
    ${INCLUDE_ROOT}/d3d12_mesh.h
    ${INCLUDE_ROOT}/d3d12_mesh_manager.h
    ${INCLUDE_ROOT}/d3d12_render_target.h
    ${INCLUDE_ROOT}/d3d12_renderer.h
    ${INCLUDE_ROOT}/d3d12_structured_buffer.h
    ${INCLUDE_ROOT}/d3d12_texture.h
    ${INCLUDE_ROOT}/d3d12_texture_manager.h
    ${INCLUDE_ROOT}/hlsl_shader_compiler.h
    d3d12_buffer.cpp
    d3d12_constant_buffer.cpp
    d3d12_constant_buffer_pool.cpp
    d3d12_context.cpp
    d3d12_cube_map.cpp
    d3d12_descriptor_handle.cpp
    d3d12_descriptor_manager.cpp
    d3d12_material.cpp
    d3d12_mesh.cpp
    d3d12_mesh_manager.cpp
    d3d12_render_target.cpp
    d3d12_renderer.cpp
    d3d12_structured_buffer.cpp
    d3d12_texture.cpp
    d3d12_texture_manager.cpp
    hlsl_shader_compiler.cpp)
