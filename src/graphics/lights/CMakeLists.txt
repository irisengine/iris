set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/lights")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/ambient_light.h
    ${INCLUDE_ROOT}/directional_light.h
    ${INCLUDE_ROOT}/light.h
    ${INCLUDE_ROOT}/light_type.h
    ${INCLUDE_ROOT}/lighting_rig.h
    ${INCLUDE_ROOT}/point_light.h
    ambient_light.cpp
    directional_light.cpp
    point_light.cpp)
