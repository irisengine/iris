set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/graphics")

if(GRAPHICS_API MATCHES "OPENGL")
    add_subdirectory("gl")
elseif(GRAPHICS_API MATCHES "METAL")
    add_subdirectory("metal")
endif()

if(PLATFORM MATCHES "OSX")
    add_subdirectory("osx")
endif()

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/buffer.hpp"
    "${INCLUDE_ROOT}/buffer_type.hpp"
    "${INCLUDE_ROOT}/font.hpp"
    "${INCLUDE_ROOT}/material.hpp"
    "${INCLUDE_ROOT}/material_factory.hpp"
    "${INCLUDE_ROOT}/mesh.hpp"
    "${INCLUDE_ROOT}/render_system.hpp"
    "${INCLUDE_ROOT}/texture.hpp"
    "${INCLUDE_ROOT}/vertex_data.hpp"
    "${SOURCE_ROOT}/shape_factory.cpp" "${INCLUDE_ROOT}/shape_factory.hpp"
    "${SOURCE_ROOT}/sprite.cpp" "${INCLUDE_ROOT}/sprite.hpp"
    "${SOURCE_ROOT}/utility.cpp" "${INCLUDE_ROOT}/utility.hpp"

    PARENT_SCOPE
)


