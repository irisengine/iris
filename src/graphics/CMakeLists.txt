set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/graphics")

if(GRAPHICS_API MATCHES "OPENGL")
    set(GRAPHICS_SRCS
        "${INCLUDE_ROOT}/gl/shader_type.hpp"
        "${SOURCE_ROOT}/gl/buffer.cpp"
        "${SOURCE_ROOT}/gl/material.cpp"
        "${SOURCE_ROOT}/gl/material_factory.cpp"
        "${SOURCE_ROOT}/gl/mesh.cpp"
        "${SOURCE_ROOT}/gl/opengl.cpp" "${INCLUDE_ROOT}/gl/opengl.hpp"
        "${SOURCE_ROOT}/gl/render_system.cpp"
        "${SOURCE_ROOT}/gl/shader.cpp" "${INCLUDE_ROOT}/gl/shader.hpp"
        "${SOURCE_ROOT}/gl/texture.cpp"
    )
elseif(GRAPHICS_API MATCHES "METAL")
    set(GRAPHICS_SRCS
        "${SOURCE_ROOT}/metal/buffer.mm"
        "${SOURCE_ROOT}/metal/material.mm"
        "${SOURCE_ROOT}/metal/material_factory.cpp"
        "${SOURCE_ROOT}/metal/mesh.mm"
        "${SOURCE_ROOT}/metal/render_system.mm"
        "${SOURCE_ROOT}/metal/texture.mm"
    )
endif()

if(PLATFORM MATCHES "OSX")
    set(PLATFORM_SRCS
        "${INCLUDE_ROOT}/osx/cf_ptr.hpp"
        "${SOURCE_ROOT}/osx/font.mm"
    )
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


