cmake_minimum_required(VERSION 3.18)

project(
  iris
  VERSION "3.0.0"
  DESCRIPTION "Cross-platform game engine"
  LANGUAGES C CXX)

include(CMakePackageConfigHelpers)
include(FetchContent)
include(GNUInstallDirs)
include(GenerateExportHeader)

# set options for library
option(IRIS_BUILD_UNIT_TESTS "whether to build unit tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(ASM_OPTIONS "-x assembler-with-cpp")

# if a platform wasn't supplied then default to current platform
if(NOT IRIS_PLATFORM)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(IRIS_PLATFORM "MACOS")
    set(IRIS_ARCH "X86_64")
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(IRIS_PLATFORM "WIN32")
    set(IRIS_ARCH "X86_64")
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(IRIS_PLATFORM "LINUX")
    set(IRIS_ARCH "X86_64")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
endif()

if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
  enable_language(OBJC)
  enable_language(OBJCXX)
  enable_language(ASM)
endif()

if(IRIS_PLATFORM MATCHES "LINUX")
  enable_language(ASM)
endif()

# set options for third party libraries
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
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

# fetch third party libraries
# note that in most cases we manually populate and add, this alloes us to use
# EXCLUDE_FROM_ALL to prevent them from being included in the install step

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG v5.0.1)
FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
  FetchContent_Populate(assimp)
  add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3
  GIT_TAG 3.17)
FetchContent_GetProperties(bullet)
if(NOT bullet_POPULATED)
  FetchContent_Populate(bullet)
  add_subdirectory(${bullet_SOURCE_DIR} ${bullet_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# stb doesn't have a cmake file, so just make it available
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG c0c982601f40183e74d84a61237e968dca08380e
  CONFIGURE_COMMAND "" BUILD_COMMAND "")
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
  lua
  GIT_REPOSITORY https://github.com/lua/lua.git
  GIT_TAG v5.4.3
  CONFIGURE_COMMAND "" BUILD_COMMAND "")
FetchContent_MakeAvailable(lua)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
  inja
  GIT_REPOSITORY https://github.com/pantor/inja
  GIT_TAG v3.3.0)
FetchContent_GetProperties(inja)
if(NOT inja_POPULATED)
  FetchContent_Populate(inja)
  add_subdirectory(${inja_SOURCE_DIR} ${inja_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if(IRIS_PLATFORM MATCHES "WIN32")
  FetchContent_Declare(
    directx-headers
    GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
    GIT_TAG v1.4.9)
 FetchContent_GetProperties(directx-headers)
 if(NOT directx-headers_POPULATED)
   FetchContent_Populate(directx-headers)
   add_subdirectory(${directx-headers_SOURCE_DIR} ${directx-headers_BINARY_DIR} EXCLUDE_FROM_ALL)
 endif()
endif()

add_subdirectory("shaders")
add_subdirectory("src")
add_subdirectory("samples")

if(IRIS_BUILD_UNIT_TESTS)
  enable_testing()
  include(CTest)
  #add_subdirectory("tests")
endif()

include(cmake/cpack.cmake)
