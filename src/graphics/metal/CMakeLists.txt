set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/buffer.mm"
    "${SOURCE_ROOT}/compiler.cpp"
    "${SOURCE_ROOT}/material.mm"
    "${SOURCE_ROOT}/mesh.mm"
    "${SOURCE_ROOT}/render_system.mm"
    "${SOURCE_ROOT}/render_target.mm"
    "${SOURCE_ROOT}/texture.mm"
    PARENT_SCOPE)
