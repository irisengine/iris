set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/physics")

add_subdirectory("bullet")

set(PHYSICS_SRCS
    ${PHYSICS_SRCS}
    "${INCLUDE_ROOT}/basic_character_controller.h"
    "${INCLUDE_ROOT}/character_controller.h"
    "${INCLUDE_ROOT}/collision_shape.h"
    "${INCLUDE_ROOT}/physics_system.h"
    "${INCLUDE_ROOT}/rigid_body.h"
    "${INCLUDE_ROOT}/rigid_body_type.h"

    PARENT_SCOPE
)


