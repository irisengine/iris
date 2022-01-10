set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/physics")

add_subdirectory("bullet")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/basic_character_controller.h
    ${INCLUDE_ROOT}/character_controller.h
    ${INCLUDE_ROOT}/collision_shape.h
    ${INCLUDE_ROOT}/contact_point.h
    ${INCLUDE_ROOT}/physics_manager.h
    ${INCLUDE_ROOT}/physics_system.h
    ${INCLUDE_ROOT}/rigid_body.h
    ${INCLUDE_ROOT}/rigid_body_type.h)
