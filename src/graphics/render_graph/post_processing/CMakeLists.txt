set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/render_graph/post_processing")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/anti_aliasing_node.h
    ${INCLUDE_ROOT}/colour_adjust_node.h
    anti_aliasing_node.cpp
    colour_adjust_node.cpp)
