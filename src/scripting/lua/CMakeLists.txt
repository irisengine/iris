set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/scripting/lua")

add_subdirectory(interop)

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/lua_script.h
    lua_script.cpp)
