set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/physics/bullet")

set(PHYSICS_SRCS
    "${SOURCE_ROOT}/basic_character_controller.cpp"
    "${SOURCE_ROOT}/box_rigid_body.cpp"
    "${SOURCE_ROOT}/capsule_rigid_body.cpp"
    "${SOURCE_ROOT}/physics_system.cpp"

    PARENT_SCOPE
)

