set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/physics/bullet")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/debug_draw.h
    basic_character_controller.cpp
    box_collision_shape.cpp
    capsule_collision_shape.cpp
    debug_draw.cpp
    physics_system.cpp
    rigid_body.cpp)

