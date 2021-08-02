set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_SRCS
    ${GRAPHICS_SRCS}
    "${SOURCE_ROOT}/ambient_light.cpp"
    "${SOURCE_ROOT}/directional_light.cpp"
    "${SOURCE_ROOT}/point_light.cpp"
    PARENT_SCOPE)
