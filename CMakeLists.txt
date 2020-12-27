cmake_minimum_required(VERSION 3.9)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

project(
  iris
  VERSION "0.1.0"
  DESCRIPTION "Cross-platform game engine"
  LANGUAGES C CXX OBJCXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(ASM_OPTIONS "-x assembler-with-cpp")

# if a platform wasn't supplied then default to current platform
if(NOT IRIS_PLATFORM)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(IRIS_PLATFORM "MACOS")
  else()
    message(FATAL_ERROR "Unsupported platform")
  endif()
endif()

add_subdirectory("third_party")
add_subdirectory("src")
add_subdirectory("samples")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  enable_testing()
  include(CTest)
  add_subdirectory("tests")
endif()
