set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics")

if(IRIS_GRAPHICS_API MATCHES "OPENGL")
    add_subdirectory("gl")
elseif(IRIS_GRAPHICS_API MATCHES "METAL")
    add_subdirectory("metal")
endif()

if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
    add_subdirectory("macos")
endif()

add_subdirectory("render_graph")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    ${PLATFORM_SRCS}
    ${RENDER_GRAPH_SRCS}
    "${INCLUDE_ROOT}/buffer.h"
    "${INCLUDE_ROOT}/buffer_descriptor.h"
    "${INCLUDE_ROOT}/buffer_type.h"
    "${INCLUDE_ROOT}/font.h"
    "${INCLUDE_ROOT}/keyframe.h"
    "${INCLUDE_ROOT}/material.h"
    "${INCLUDE_ROOT}/material_factory.h"
    "${INCLUDE_ROOT}/primitive_type.h"
    "${INCLUDE_ROOT}/render_system.h"
    "${INCLUDE_ROOT}/render_target.h"
    "${INCLUDE_ROOT}/texture.h"
    "${INCLUDE_ROOT}/vertex_data.h"
    "${INCLUDE_ROOT}/weight.h"
    "${SOURCE_ROOT}/animation.cpp" "${INCLUDE_ROOT}/animation.h"
    "${SOURCE_ROOT}/bone.cpp" "${INCLUDE_ROOT}/bone.h"
    "${SOURCE_ROOT}/mesh_factory.cpp" "${INCLUDE_ROOT}/mesh_factory.h"
    "${SOURCE_ROOT}/mesh_loader.cpp" "${INCLUDE_ROOT}/mesh_loader.h"
    "${SOURCE_ROOT}/render_entity.cpp" "${INCLUDE_ROOT}/render_entity.h"
    "${SOURCE_ROOT}/skeleton.cpp" "${INCLUDE_ROOT}/skeleton.h"
    "${SOURCE_ROOT}/texture_factory.cpp" "${INCLUDE_ROOT}/texture_factory.h"
    "${SOURCE_ROOT}/vertex_attributes.cpp" "${INCLUDE_ROOT}/vertex_attributes.h"

    PARENT_SCOPE
)


