set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/physics")

add_subdirectory("bullet")

set(PHYSICS_SRCS
    ${PHYSICS_SRCS}
    "${INCLUDE_ROOT}/box_rigid_body.h"
    "${INCLUDE_ROOT}/rigid_body.h"

    PARENT_SCOPE
)


