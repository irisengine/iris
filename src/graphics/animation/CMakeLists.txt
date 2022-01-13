set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/graphics/animation")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/animation.h
    ${INCLUDE_ROOT}/animation_controller.h
    ${INCLUDE_ROOT}/animation_layer.h
    ${INCLUDE_ROOT}/animation_state.h
    ${INCLUDE_ROOT}/animation_transition.h
    ${INCLUDE_ROOT}/bone_query.h
    ${INCLUDE_ROOT}/cached_bone_query.h
    ${INCLUDE_ROOT}/utils.h
    animation.cpp
    animation_controller.cpp
    animation_state.cpp
    cached_bone_query.cpp
    utils.cpp)
