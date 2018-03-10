SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/physics")

SET(PHYSICS_SRCS
    "${INCLUDE_ROOT}/rigid_body_shape.hpp"
    "${SOURCE_ROOT}/box.cpp" "${INCLUDE_ROOT}/box.hpp"
    "${SOURCE_ROOT}/collision_detector.cpp" "${INCLUDE_ROOT}/collision_resolver.hpp"
    "${SOURCE_ROOT}/collision_resolver.cpp" "${INCLUDE_ROOT}/collision_resolver.hpp"
    "${SOURCE_ROOT}/contact.cpp" "${INCLUDE_ROOT}/contact.hpp"
    "${SOURCE_ROOT}/physics_system.cpp" "${INCLUDE_ROOT}/physics_system.hpp"
    "${SOURCE_ROOT}/plane.cpp" "${INCLUDE_ROOT}/plane.hpp"
    "${SOURCE_ROOT}/rigid_body.cpp" "${INCLUDE_ROOT}/rigid_body.hpp"

    PARENT_SCOPE
)

