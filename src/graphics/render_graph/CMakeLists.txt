set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/render_graph")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/arithmetic_node.h
    ${INCLUDE_ROOT}/blur_node.h
    ${INCLUDE_ROOT}/colour_node.h
    ${INCLUDE_ROOT}/combine_node.h
    ${INCLUDE_ROOT}/component_node.h
    ${INCLUDE_ROOT}/composite_node.h
    ${INCLUDE_ROOT}/invert_node.h
    ${INCLUDE_ROOT}/node.h
    ${INCLUDE_ROOT}/render_graph.h
    ${INCLUDE_ROOT}/render_node.h
    ${INCLUDE_ROOT}/shader_compiler.h
    ${INCLUDE_ROOT}/sin_node.h
    ${INCLUDE_ROOT}/texture_node.h
    ${INCLUDE_ROOT}/utils.h
    ${INCLUDE_ROOT}/vertex_position_node.h
    arithmetic_node.cpp
    blur_node.cpp
    colour_node.cpp
    combine_node.cpp
    component_node.cpp
    composite_node.cpp
    invert_node.cpp
    render_graph.cpp
    render_node.cpp
    sin_node.cpp
    texture_node.cpp
    utils.cpp
    vertex_position_node.cpp)
