add_subdirectory("core")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("log")
add_subdirectory("networking")
add_subdirectory("physics")
add_subdirectory("platform")

add_library(iris STATIC
    ${CORE_SRCS}
    ${GRAPHICS_SRCS}
    ${JOB_SRCS}
    ${LOG_SRCS}
    ${NETWORKING_SRCS}
    ${PHYSICS_SRCS}
    ${PLATFORM_SRCS}
)

target_include_directories(iris PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")

target_include_directories(iris SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
target_include_directories(iris SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party/bullet/src")
target_include_directories(iris SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party/assimp/include")
target_include_directories(iris SYSTEM PRIVATE "${CMAKE_BINARY_DIR}/third_party/assimp/include")

target_link_libraries(iris LinearMath BulletDynamics BulletCollision assimp)
target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fno-rtti -fobjc-arc)
