set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/render_graph")

add_subdirectory("post_processing")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/binary_operator_node.h
    ${INCLUDE_ROOT}/blur_node.h
    ${INCLUDE_ROOT}/colour_node.h
    ${INCLUDE_ROOT}/combine_node.h
    ${INCLUDE_ROOT}/component_node.h
    ${INCLUDE_ROOT}/composite_node.h
    ${INCLUDE_ROOT}/conditional_node.h
    ${INCLUDE_ROOT}/invert_node.h
    ${INCLUDE_ROOT}/lerp_node.h
    ${INCLUDE_ROOT}/node.h
    ${INCLUDE_ROOT}/property.h
    ${INCLUDE_ROOT}/property_node.h
    ${INCLUDE_ROOT}/property_writer.h
    ${INCLUDE_ROOT}/render_graph.h
    ${INCLUDE_ROOT}/render_node.h
    ${INCLUDE_ROOT}/shader_compiler.h
    ${INCLUDE_ROOT}/sky_box_node.h
    ${INCLUDE_ROOT}/texture_node.h
    ${INCLUDE_ROOT}/time_node.h
    ${INCLUDE_ROOT}/unary_operator_node.h
    ${INCLUDE_ROOT}/variable_node.h
    ${INCLUDE_ROOT}/vertex_node.h
    binary_operator_node.cpp
    blur_node.cpp
    colour_node.cpp
    combine_node.cpp
    component_node.cpp
    composite_node.cpp
    conditional_node.cpp
    invert_node.cpp
    lerp_node.cpp
    property.cpp
    property_node.cpp
    render_graph.cpp
    render_node.cpp
    shader_compiler.cpp
    sky_box_node.cpp
    texture_node.cpp
    time_node.cpp
    unary_operator_node.cpp
    variable_node.cpp
    vertex_node.cpp
)
