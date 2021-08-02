set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics")

if(IRIS_GRAPHICS_API MATCHES "OPENGL")
  add_subdirectory("opengl")
elseif(IRIS_GRAPHICS_API MATCHES "METAL")
  add_subdirectory("metal")
endif()

if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
  add_subdirectory("macos")
endif()

add_subdirectory("lights")
add_subdirectory("render_graph")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${INCLUDE_ROOT}/animation.h"
    "${INCLUDE_ROOT}/bone.h"
    "${INCLUDE_ROOT}/buffer.h"
    "${INCLUDE_ROOT}/buffer_type.h"
    "${INCLUDE_ROOT}/keyframe.h"
    "${INCLUDE_ROOT}/material.h"
    "${INCLUDE_ROOT}/mesh.h"
    "${INCLUDE_ROOT}/mesh_factory.h"
    "${INCLUDE_ROOT}/mesh_loader.h"
    "${INCLUDE_ROOT}/pipeline.h"
    "${INCLUDE_ROOT}/pixel_format.h"
    "${INCLUDE_ROOT}/primitive_type.h"
    "${INCLUDE_ROOT}/render_entity.h"
    "${INCLUDE_ROOT}/render_system.h"
    "${INCLUDE_ROOT}/render_target.h"
    "${INCLUDE_ROOT}/scene.h"
    "${INCLUDE_ROOT}/skeleton.h"
    "${INCLUDE_ROOT}/stage.h"
    "${INCLUDE_ROOT}/text_factory.h"
    "${INCLUDE_ROOT}/texture.h"
    "${INCLUDE_ROOT}/texture_factory.h"
    "${INCLUDE_ROOT}/vertex_attributes.h"
    "${INCLUDE_ROOT}/vertex_data.h"
    "${INCLUDE_ROOT}/weight.h"
    "${SOURCE_ROOT}/animation.cpp"
    "${SOURCE_ROOT}/bone.cpp"
    "${SOURCE_ROOT}/mesh_factory.cpp"
    "${SOURCE_ROOT}/mesh_loader.cpp"
    "${SOURCE_ROOT}/mesh_manager.cpp"
    "${SOURCE_ROOT}/render_command.cpp"
    "${SOURCE_ROOT}/render_entity.cpp"
    "${SOURCE_ROOT}/scene.cpp"
    "${SOURCE_ROOT}/skeleton.cpp"
    "${SOURCE_ROOT}/stage.cpp"
    "${SOURCE_ROOT}/texture_factory.cpp"
    "${SOURCE_ROOT}/vertex_attributes.cpp"
    "${SOURCE_ROOT}/window.cpp"
    PARENT_SCOPE)
