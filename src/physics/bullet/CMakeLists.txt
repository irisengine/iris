set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/physics/bullet")

set(PHYSICS_SRCS
    "${SOURCE_ROOT}/basic_character_controller.cpp"
    "${SOURCE_ROOT}/box_collision_shape.cpp"
    "${SOURCE_ROOT}/capsule_collision_shape.cpp"
    "${SOURCE_ROOT}/physics_system.cpp"
    "${INCLUDE_ROOT}/debug_draw.h" "${SOURCE_ROOT}/debug_draw.cpp"
    "${SOURCE_ROOT}/rigid_body.cpp"

    PARENT_SCOPE
)

