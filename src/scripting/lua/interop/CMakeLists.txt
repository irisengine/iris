set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/scripting/lua/interop")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/class_interop.h
    ${INCLUDE_ROOT}/register_class.h
    ${INCLUDE_ROOT}/quaternion.h
    ${INCLUDE_ROOT}/vector3.h
    register_class.cpp
    quaternion.cpp
    vector3.cpp)
