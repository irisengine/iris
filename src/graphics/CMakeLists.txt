set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("macos")
  add_subdirectory("metal")
elseif(IRIS_PLATFORM MATCHES "IOS")
  add_subdirectory("ios")
  add_subdirectory("metal")
elseif(IRIS_PLATFORM MATCHES "WIN32")
  add_subdirectory("win32")
  add_subdirectory("opengl")
  add_subdirectory("d3d12")
elseif(IRIS_PLATFORM MATCHES "LINUX")
  add_subdirectory("linux")
  add_subdirectory("opengl")
endif()

add_subdirectory("animation")
add_subdirectory("lights")
add_subdirectory("render_graph")

target_sources(iris PRIVATE
  ${INCLUDE_ROOT}/bone.h
  ${INCLUDE_ROOT}/cube_map.h
  ${INCLUDE_ROOT}/default_shader_languages.h
  ${INCLUDE_ROOT}/instanced_entity.h
  ${INCLUDE_ROOT}/keyframe.h
  ${INCLUDE_ROOT}/material.h
  ${INCLUDE_ROOT}/material_cache.h
  ${INCLUDE_ROOT}/material_manager.h
  ${INCLUDE_ROOT}/mesh.h
  ${INCLUDE_ROOT}/mesh_loader.h
  ${INCLUDE_ROOT}/mesh_manager.h
  ${INCLUDE_ROOT}/post_processing_description.h
  ${INCLUDE_ROOT}/primitive_type.h
  ${INCLUDE_ROOT}/render_command.h
  ${INCLUDE_ROOT}/render_command_type.h
  ${INCLUDE_ROOT}/render_entity.h
  ${INCLUDE_ROOT}/render_entity_type.h
  ${INCLUDE_ROOT}/render_pass.h
  ${INCLUDE_ROOT}/render_pipeline.h
  ${INCLUDE_ROOT}/render_target.h
  ${INCLUDE_ROOT}/render_target_manager.h
  ${INCLUDE_ROOT}/renderer.h
  ${INCLUDE_ROOT}/sampler.h
  ${INCLUDE_ROOT}/scene.h
  ${INCLUDE_ROOT}/single_entity.h
  ${INCLUDE_ROOT}/skeleton.h
  ${INCLUDE_ROOT}/text_factory.h
  ${INCLUDE_ROOT}/texture.h
  ${INCLUDE_ROOT}/texture_manager.h
  ${INCLUDE_ROOT}/texture_usage.h
  ${INCLUDE_ROOT}/utils.h
  ${INCLUDE_ROOT}/vertex_attributes.h
  ${INCLUDE_ROOT}/vertex_data.h
  ${INCLUDE_ROOT}/weight.h
  ${INCLUDE_ROOT}/window.h
  ${INCLUDE_ROOT}/window_manager.h
  bone.cpp
  cube_map.cpp
  instanced_entity.cpp
  material.cpp
  material_manager.cpp
  mesh.cpp
  mesh_loader.cpp
  mesh_manager.cpp
  render_command.cpp
  render_entity.cpp
  render_pipeline.cpp
  render_target.cpp
  renderer.cpp
  sampler.cpp
  scene.cpp
  single_entity.cpp
  skeleton.cpp
  texture.cpp
  texture_manager.cpp
  utils.cpp
  vertex_attributes.cpp
  window.cpp
)
