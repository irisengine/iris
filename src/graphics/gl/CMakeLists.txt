set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/gl")

set(GRAPHICS_SRCS
    "${INCLUDE_ROOT}/shader_type.h"
    "${SOURCE_ROOT}/buffer.cpp"
    "${SOURCE_ROOT}/buffer_descriptor.cpp"
    "${SOURCE_ROOT}/compiler.cpp"
    "${SOURCE_ROOT}/material.cpp"
    "${SOURCE_ROOT}/material_factory.cpp"
    "${SOURCE_ROOT}/render_target.cpp"
    "${SOURCE_ROOT}/opengl.cpp" "${INCLUDE_ROOT}/opengl.h"
    "${SOURCE_ROOT}/render_system.cpp"
    "${SOURCE_ROOT}/shader.cpp" "${INCLUDE_ROOT}/shader.h"
    "${SOURCE_ROOT}/texture.cpp"

    PARENT_SCOPE
)

