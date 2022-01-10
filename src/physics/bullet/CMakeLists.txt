set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/physics/bullet")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/bullet_box_collision_shape.h
    ${INCLUDE_ROOT}/bullet_capsule_collision_shape.h
    ${INCLUDE_ROOT}/bullet_collision_shape.h
    ${INCLUDE_ROOT}/bullet_mesh_collision_shape.h
    ${INCLUDE_ROOT}/bullet_physics_manager.h
    ${INCLUDE_ROOT}/bullet_physics_system.h
    ${INCLUDE_ROOT}/bullet_rigid_body.h
    ${INCLUDE_ROOT}/collision_callback.h
    ${INCLUDE_ROOT}/debug_draw.h
    basic_character_controller.cpp
    bullet_box_collision_shape.cpp
    bullet_capsule_collision_shape.cpp
    bullet_mesh_collision_shape.cpp
    bullet_physics_manager.cpp
    bullet_physics_system.cpp
    bullet_rigid_body.cpp
    collision_callback
    debug_draw.cpp)
