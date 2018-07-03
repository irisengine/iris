SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/graphics")

IF(GRAPHICS_API MATCHES "OPENGL")
    SET(GRAPHICS_SRCS
        "${INCLUDE_ROOT}/gl/shader_type.hpp"
        "${SOURCE_ROOT}/gl/buffer.cpp" "${INCLUDE_ROOT}/gl/buffer.hpp"
        "${SOURCE_ROOT}/gl/material.cpp"
        "${SOURCE_ROOT}/gl/mesh_implementation.cpp" "${INCLUDE_ROOT}/gl/mesh_implementation.hpp"
        "${SOURCE_ROOT}/gl/opengl.cpp" "${INCLUDE_ROOT}/gl/opengl.hpp"
        "${SOURCE_ROOT}/gl/render_system.cpp"
        "${SOURCE_ROOT}/gl/shader.cpp" "${INCLUDE_ROOT}/gl/shader.hpp"
        "${SOURCE_ROOT}/gl/texture_implementation.cpp" "${INCLUDE_ROOT}/gl/texture_implementation.hpp"
        "${SOURCE_ROOT}/gl/vertex_state.cpp" "${INCLUDE_ROOT}/gl/vertex_state.hpp"
    )
ELSEIF(GRAPHICS_API MATCHES "METAL")
    SET(GRAPHICS_SRCS
        "${SOURCE_ROOT}/metal/buffer.mm" "${INCLUDE_ROOT}/metal/buffer.hpp"
        "${SOURCE_ROOT}/metal/material.mm"
        "${SOURCE_ROOT}/metal/mesh_implementation.mm" "${INCLUDE_ROOT}/metal/mesh_implementation.hpp"
        "${SOURCE_ROOT}/metal/render_system.mm"
        "${SOURCE_ROOT}/metal/texture_implementation.mm" "${INCLUDE_ROOT}/metal/texture_implementation.hpp"
    )
ENDIF()

SET(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${INCLUDE_ROOT}/auto_bind.hpp"
    "${INCLUDE_ROOT}/material.hpp"
    "${INCLUDE_ROOT}/render_system.hpp"
    "${INCLUDE_ROOT}/vertex_data.hpp"
    "${SOURCE_ROOT}/entity.cpp" "${INCLUDE_ROOT}/entity.hpp"
    "${SOURCE_ROOT}/mesh.cpp" "${INCLUDE_ROOT}/mesh.hpp"
    "${SOURCE_ROOT}/shape_factory.cpp" "${INCLUDE_ROOT}/shape_factory.hpp"
    "${SOURCE_ROOT}/texture.cpp" "${INCLUDE_ROOT}/texture.hpp"

    PARENT_SCOPE
)


