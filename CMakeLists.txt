cmake_minimum_required(VERSION 3.14)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

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
    set(IRIS_ARCH "X86_64")
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(IRIS_PLATFORM "WINDOWS")
    set(IRIS_ARCH "X86_64")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
endif()

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG v5.0.1)

FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3
  GIT_TAG 3.17)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG c0c982601f40183e74d84a61237e968dca08380e
  CONFIGURE_COMMAND "" BUILD_COMMAND "")

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0)

 FetchContent_GetProperties(assimp)
 if(NOT assimp_POPULATED)
   FetchContent_Populate(assimp)
   add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR} EXCLUDE_FROM_ALL)
 endif()

 FetchContent_GetProperties(bullet)
 if(NOT bullet_POPULATED)
   FetchContent_Populate(bullet)
   add_subdirectory(${bullet_SOURCE_DIR} ${bullet_BINARY_DIR} EXCLUDE_FROM_ALL)
 endif()

 FetchContent_MakeAvailable(stb)

 FetchContent_GetProperties(googletest)
 if(NOT googletest_POPULATED)
   FetchContent_Populate(googletest)
   add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
 endif()

add_subdirectory("src")
add_subdirectory("samples")

enable_testing()
include(CTest)
add_subdirectory("tests")
