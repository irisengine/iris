set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/networking")

add_subdirectory("posix")

set(NETWORKING_SRCS
    ${NETWORKING_SRCS}


    PARENT_SCOPE
)
