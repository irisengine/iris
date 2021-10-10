add_library(iris STATIC "")

add_subdirectory("core")
add_subdirectory("events")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("log")
add_subdirectory("networking")
add_subdirectory("physics")

add_library(iris::iris ALIAS iris)
generate_export_header(iris)

# hide symbols
set_target_properties(iris PROPERTIES
  CMAKE_CXX_VISIBILITY_PRESET hidden
  CMAKE_VISIBILITY_INLINES_HIDDEN 1)

target_include_directories(
  iris
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/iris>
         $<INSTALL_INTERFACE:include/iris>
  PRIVATE ${stb_SOURCE_DIR} ${bullet_SOURCE_DIR}/src)

# configure ersion file
configure_file(${CMAKE_SOURCE_DIR}/include/iris/iris_version.h.in ${CMAKE_SOURCE_DIR}/include/iris/iris_version.h)

# set macos/ios framework includes
if(IRIS_PLATFORM MATCHES "MACOS")
  target_link_libraries(iris PUBLIC "-framework AppKit -framework CoreFoundation -framework CoreGraphics -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore")
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_link_libraries(iris PUBLIC "-framework AppKit -framework CoreFoundation -framework UIKit -framework Foundation -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore")
endif()

# handle platform specific setup including setting default graphics apis
if(IRIS_PLATFORM MATCHES "MACOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_MACOS)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_X86_64)
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "FIBERS")
  endif()
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_IOS)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_ARM64)
  set(IRIS_ARCH "ARM64")
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
elseif(IRIS_PLATFORM MATCHES "WIN32")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_WIN32)
  target_compile_definitions(iris PUBLIC IRIS_ARCH_X86_64)
  target_compile_definitions(iris PUBLIC NOMINMAX)
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
else()
  message(FATAL_ERROR "Unsupported platform")
endif()


if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "WINDOWS")
  find_package(OpenGL REQUIRED)
  target_include_directories(iris SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
  if(IRIS_PLATFORM MATCHES "MACOS")
    target_link_libraries(iris PUBLIC "-framework OpenGL")
    # opengl is technically deprecated on macos, so silence the deprecation
    # warnings
    target_compile_definitions(iris PRIVATE GL_SILENCE_DEPRECATION)
  endif()
endif()

message(STATUS "Building iris-${CMAKE_PROJECT_VERSION} for ${IRIS_PLATFORM} (${IRIS_JOBS_API})")

target_link_libraries(iris PUBLIC LinearMath BulletDynamics BulletCollision assimp)

if(IRIS_PLATFORM MATCHES "WIN32")
  # on windows SYSTEM includes do not prevent warnings - so we add bullet using
  # the /external flag
  target_compile_options(iris PRIVATE /W4 /WX /experimental:external
                                      /external:W0 /external:I ${bullet_SOURCE_DIR})
  target_link_options(iris PUBLIC /subsystem:windows /ENTRY:mainCRTStartup)
  target_link_libraries(iris PRIVATE DirectX-Headers)

  set_target_properties(iris PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(assimp PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(LinearMath PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(BulletDynamics PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(BulletCollision PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")

  install(
    TARGETS iris
            LinearMath
            BulletDynamics
            BulletCollision
            assimp
            zlibstatic
            IrrXML
            DirectX-Headers
    EXPORT iris-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
  target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fobjc-arc)

  install(
    TARGETS iris assimp zlibstatic IrrXML BulletDynamics BulletCollision LinearMath
    EXPORT iris-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# various install commands to ensure all files get put in the right place

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/iris
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/iris-config.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/iris-config.cmake
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

install(FILES ${CMAKE_BINARY_DIR}/cmake/iris-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/iris-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/iris_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/iris)

export(EXPORT iris-targets
  FILE ${CMAKE_BINARY_DIR}/cmake/iris-targets.cmake
  NAMESPACE iris::)
