add_subdirectory("core")
add_subdirectory("graphics")
add_subdirectory("log")
add_subdirectory("platform")

add_library(iris STATIC ${CORE_SRCS} ${GRAPHICS_SRCS} ${LOG_SRCS} ${PHYSICS_SRCS} ${PLATFORM_SRCS} ${SCRIPT_SRCS})

target_include_directories(iris PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_include_directories(iris PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
