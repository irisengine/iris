add_library(iris STATIC "")

add_subdirectory("core")
add_subdirectory("events")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("log")
add_subdirectory("networking")
add_subdirectory("physics")
add_subdirectory("scripting")

add_library(iris::iris ALIAS iris)
generate_export_header(iris)

target_compile_features(iris PUBLIC cxx_std_20)

# hide symbols
set_target_properties(iris PROPERTIES
  CMAKE_CXX_VISIBILITY_PRESET hidden
  CMAKE_VISIBILITY_INLINES_HIDDEN 1)

target_include_directories(
  iris
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/iris>
  $<INSTALL_INTERFACE:include/iris>)

target_include_directories(
  iris SYSTEM
  PRIVATE ${stb_SOURCE_DIR} ${bullet_SOURCE_DIR}/src ${lua_SOURCE_DIR} ${inja_SOURCE_DIR}/include ${inja_SOURCE_DIR}/third_party/include)

target_include_directories(
  iris SYSTEM
  PRIVATE ${PROJECT_BINARY_DIR}/shaders)

# lua does not use cmake, so we build it as a separate library
add_library(lua STATIC ${lua_SOURCE_DIR}/onelua.c)
target_compile_definitions(lua PRIVATE MAKE_LIB)

# configure version file
configure_file(${PROJECT_SOURCE_DIR}/include/iris/iris_version.h.in ${PROJECT_SOURCE_DIR}/include/iris/iris_version.h)

# set macos/ios framework includes
if(IRIS_PLATFORM MATCHES "MACOS")
  target_link_libraries(iris PUBLIC "-framework AppKit -framework CoreFoundation -framework CoreGraphics -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore")
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_link_libraries(iris PUBLIC "-framework AppKit -framework CoreFoundation -framework UIKit -framework Foundation -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore")
endif()

# default link options (maybe extended by platform below)
set(IRIS_LINKED_LIBS IrrXML zlibstatic BulletDynamics BulletCollision LinearMath assimp lua)
set(IRIS_LINKED_LIBS_PRIVATE)

# handle platform specific setup including setting default graphics apis
if(IRIS_PLATFORM MATCHES "MACOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_MACOS)

  if(IRIS_ARCH MATCHES "ARM64")
    target_compile_definitions(iris PUBLIC IRIS_ARCH_ARM64)
  else()
    target_compile_definitions(iris PUBLIC IRIS_ARCH_X86_64)
  endif()

  target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fobjc-arc)
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_IOS)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_ARM64)
  target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fobjc-arc)
  set(IRIS_ARCH "ARM64")
elseif(IRIS_PLATFORM MATCHES "WIN32")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_WIN32)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_X86_64)
  target_compile_definitions(iris PUBLIC NOMINMAX)

  # on windows SYSTEM includes do not prevent warnings - so we add bullet using
  # the /external flag
  target_compile_options(iris PRIVATE /W4 /WX /experimental:external
    /external:W0 /external:I ${bullet_SOURCE_DIR})
  target_link_options(iris PUBLIC /subsystem:windows /ENTRY:mainCRTStartup)

  list(APPEND IRIS_LINKED_LIBS DirectX-Headers)

  set_target_properties(iris PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(assimp PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(inja PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(LinearMath PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(BulletDynamics PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(BulletCollision PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(IrrXML PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(zlibstatic PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(lua PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
elseif(IRIS_PLATFORM MATCHES "LINUX")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_LINUX)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_X86_64)
  target_compile_options(iris PRIVATE -Wall -Werror)
  target_link_options(iris PUBLIC -rdynamic)

  list(APPEND IRIS_LINKED_LIBS_PRIVATE pthread GL Xfixes X11)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

# find opengl on supported platforms
if(IRIS_PLATFORM MATCHES "LINUX" OR IRIS_PLATFORM MATCHES "WINDOWS")
  find_package(OpenGL REQUIRED)
  target_include_directories(iris SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
endif()

message(STATUS "Building iris-${CMAKE_PROJECT_VERSION} for ${IRIS_PLATFORM})")

target_link_libraries(iris PUBLIC ${IRIS_LINKED_LIBS})
target_link_libraries(iris PRIVATE ${IRIS_LINKED_LIBS_PRIVATE})

# various install commands to ensure all files get put in the right place
install(
  TARGETS iris ${IRIS_LINKED_LIBS}
  EXPORT iris-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/iris
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/iris-config.cmake.in
  ${PROJECT_BINARY_DIR}/cmake/iris-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)

write_basic_package_version_file(
  iris-version.cmake
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  EXPORT iris-targets
  FILE iris-targets.cmake
  NAMESPACE iris::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)

install(
  FILES ${PROJECT_BINARY_DIR}/cmake/iris-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/iris-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/iris_export.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/iris)

export(
  EXPORT iris-targets
  FILE ${PROJECT_BINARY_DIR}/cmake/iris-targets.cmake
  NAMESPACE iris::)
