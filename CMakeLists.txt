cmake_minimum_required(VERSION 3.6)

project(iris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic -ggdb -fno-rtti -fobjc-arc")
set(ASM_OPTIONS "-x assembler-with-cpp")
enable_language(C CXX ASM)

# put all artifacts in same directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# if a platform wasn't supplied then default to current platform
if(NOT IRIS_PLATFORM)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(IRIS_PLATFORM "MACOS")
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endif()

# handle platform specific setup including setting default graphics apis
if(IRIS_PLATFORM MATCHES "MACOS")
    add_definitions(-DPLATFORM_MACOS)
    add_definitions(-DIRIS_ARCH_X86_64)
    set(IRIS_ARCH "X86_64")
    if(NOT IRIS_GRAPHICS_API)
        set(IRIS_GRAPHICS_API "METAL")
    endif()
    if(NOT IRIS_JOBS_API)
        set(IRIS_JOBS_API "fibers")
    endif()
elseif(IRIS_PLATFORM MATCHES "IOS")
    add_definitions(-DPLATFORM_IOS)
    add_definitions(-DIRIS_ARCH_ARM64)
    set(IRIS_ARCH "ARM64")
    if(NOT IRIS_GRAPHICS_API)
        set(IRIS_GRAPHICS_API "METAL")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# handle graphic api setup including validating they are supported on current
# platform
if(IRIS_GRAPHICS_API MATCHES "METAL")
    if(IRIS_PLATFORM MATCHES "MACOS" OR
       IRIS_PLATFORM MATCHES "IOS")
        add_definitions(-DIRIS_GRAPHICS_API_METAL)
    else()
        message(FATAL_ERROR "METAL not supported on ${IRIS_PLATFORM}")
    endif()
elseif(IRIS_GRAPHICS_API MATCHES "OPENGL")
    if(IRIS_PLATFORM MATCHES "MACOS")
        add_definitions(-DIRIS_GRAPHICS_API_OPENGL)
        # opengl is technically deprecated on macos, so silence the
        # deprecation warnings
        add_definitions(-DGL_SILENCE_DEPRECATION)
    else()
        message(FATAL_ERROR "OPENGL not supported on ${IRIS_PLATFORM}")
    endif()
else()
    message(FATAL_ERROR "Unsupported graphics API: ${IRIS_GRAPHICS_API}")
endif()

message(STATUS "Building for ${IRIS_PLATFORM} with ${IRIS_GRAPHICS_API}")

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_SOURCE_DIR}/samples")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")

