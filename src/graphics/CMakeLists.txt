set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/graphics")

if(IRIS_GRAPHICS_API MATCHES "OPENGL")
    add_subdirectory("gl")
elseif(IRIS_GRAPHICS_API MATCHES "METAL")
    add_subdirectory("metal")
endif()

if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
    add_subdirectory("macos")
endif()

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/buffer.h"
    "${INCLUDE_ROOT}/buffer_type.h"
    "${INCLUDE_ROOT}/font.h"
    "${INCLUDE_ROOT}/material.h"
    "${INCLUDE_ROOT}/material_factory.h"
    "${INCLUDE_ROOT}/mesh.h"
    "${INCLUDE_ROOT}/render_system.h"
    "${INCLUDE_ROOT}/texture.h"
    "${INCLUDE_ROOT}/vertex_data.h"
    "${SOURCE_ROOT}/render_entity.cpp" "${INCLUDE_ROOT}/render_entity.h"
    "${SOURCE_ROOT}/shape_factory.cpp" "${INCLUDE_ROOT}/shape_factory.h"
    "${SOURCE_ROOT}/sprite.cpp" "${INCLUDE_ROOT}/sprite.h"
    "${SOURCE_ROOT}/utility.cpp" "${INCLUDE_ROOT}/utility.h"

    PARENT_SCOPE
)


