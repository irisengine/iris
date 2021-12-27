set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/scripting")

add_subdirectory("lua")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/script.h
    ${INCLUDE_ROOT}/script_runner.h)
