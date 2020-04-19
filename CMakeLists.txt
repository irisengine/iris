cmake_minimum_required(VERSION 3.6)

project(iris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic -ggdb -fno-rtti -fobjc-arc")

# put all artifacts in same directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# set platform variable
if(NOT PLATFORM)
    # handle apple platform
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(PLATFORM "MACOS")
        add_definitions(-DPLATFORM_MACOS)

        # set graphics api, defults to metal
        if(GRAPHICS_API MATCHES "METAL" OR NOT GRAPHICS_API)
            set(GRAPHICS_API "METAL")
            add_definitions(-DGRAPHICS_API_METAL)
        elseif(GRAPHICS_API MATCHES "OPENGL")
            set(GRAPHICS_API "OPENGL")
            add_definitions(-DGRAPHICS_API_OPENGL)
            # opengl is technically deprecated on macos, so silence the
            # deprecation warnings
            add_definitions(-DGL_SILENCE_DEPRECATION)
        else()
            message(FATAL_ERROR "Unsupported graphics API")
        endif()
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endif()

message(STATUS "Building for ${PLATFORM} with ${GRAPHICS_API}")

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_SOURCE_DIR}/samples")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")

