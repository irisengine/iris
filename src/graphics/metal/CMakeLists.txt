set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    "${SOURCE_ROOT}/buffer.mm"
    "${SOURCE_ROOT}/buffer_descriptor.mm"
    "${SOURCE_ROOT}/material.mm"
    "${SOURCE_ROOT}/material_factory.cpp"
    "${SOURCE_ROOT}/render_system.mm"
    "${SOURCE_ROOT}/texture.mm"

    PARENT_SCOPE
)


