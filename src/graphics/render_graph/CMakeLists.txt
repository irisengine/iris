set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/graphics/render_graph")

set(RENDER_GRAPH_SRCS
    "${INCLUDE_ROOT}/compiler.h"
    "${INCLUDE_ROOT}/node.h"
    "${SOURCE_ROOT}/arithmetic_node.cpp" "${INCLUDE_ROOT}/arithmetic_node.h"
    "${SOURCE_ROOT}/blur_node.cpp" "${INCLUDE_ROOT}/blur_node.h"
    "${SOURCE_ROOT}/colour_node.cpp" "${INCLUDE_ROOT}/colour_node.h"
    "${SOURCE_ROOT}/combine_node.cpp" "${INCLUDE_ROOT}/combine_node.h"
    "${SOURCE_ROOT}/component_node.cpp" "${INCLUDE_ROOT}/component_node.h"
    "${SOURCE_ROOT}/composite_node.cpp" "${INCLUDE_ROOT}/composite_node.h"
    "${SOURCE_ROOT}/invert_node.cpp" "${INCLUDE_ROOT}/invert_node.h"
    "${SOURCE_ROOT}/render_node.cpp" "${INCLUDE_ROOT}/render_node.h"
    "${SOURCE_ROOT}/sin_node.cpp" "${INCLUDE_ROOT}/sin_node.h"
    "${SOURCE_ROOT}/texture_node.cpp" "${INCLUDE_ROOT}/texture_node.h"
    "${SOURCE_ROOT}/vertex_position_node.cpp" "${INCLUDE_ROOT}/vertex_position_node.h"

    PARENT_SCOPE
)


