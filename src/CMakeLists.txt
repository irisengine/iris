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

set_target_properties(iris PROPERTIES
  CMAKE_CXX_VISIBILITY_PRESET hidden
  CMAKE_VISIBILITY_INLINES_HIDDEN 1)

target_include_directories(
  iris
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/iris>
         $<INSTALL_INTERFACE:include/iris>
  PRIVATE ${stb_SOURCE_DIR} ${bullet_SOURCE_DIR}/src)

configure_file(${CMAKE_SOURCE_DIR}/include/iris/iris_version.h.in ${CMAKE_SOURCE_DIR}/include/iris/iris_version.h)

# on windows SYSTEM includes do not prevent warnings - so we add bullet using
# the /external flag
if(IRIS_PLATFORM MATCHES "WINDOWS")
  set(WINDOWS_SYSTEM_INCLUDES /external:I
                              ${CMAKE_SOURCE_DIR}/third_party/bullet/src)
endif()

if(IRIS_PLATFORM MATCHES "MACOS")
  set(LINK_FRAMEWORKS
      "-framework AppKit -framework CoreFoundation -framework CoreGraphics")
elseif(IRIS_PLATFORM MATCHES "IOS")
  set(LINK_FRAMEWORKS
      "-framework AppKit -framework CoreFoundation -framework UIKit -framework Foundation"
  )
endif()

# handle platform specific setup including setting default graphics apis
if(IRIS_PLATFORM MATCHES "MACOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_MACOS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_X86_64)
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "FIBERS")
  endif()
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_IOS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_ARM64)
  set(IRIS_ARCH "ARM64")
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_WINDOWS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_X86_64)
  target_compile_definitions(iris PUBLIC NOMINMAX)
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

# handle graphic api setup
if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
  set(LINK_FRAMEWORKS
      "${LINK_FRAMEWORKS} -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore"
  )
endif()

if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "WINDOWS")
  find_package(OpenGL REQUIRED)
  target_include_directories(iris SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
  if(IRIS_PLATFORM MATCHES "MACOS")
    set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenGL")
    # opengl is technically deprecated on macos, so silence the deprecation
    # warnings
    target_compile_definitions(iris PRIVATE GL_SILENCE_DEPRECATION)
  endif()
endif()

message(STATUS "Building for ${IRIS_PLATFORM} (${IRIS_JOBS_API})")

target_link_libraries(iris PUBLIC LinearMath BulletDynamics BulletCollision
                                  assimp ${LINK_FRAMEWORKS})

if(IRIS_PLATFORM MATCHES "WINDOWS")
  target_compile_options(iris PRIVATE /W4 /WX /experimental:external
                                      /external:W0 ${WINDOWS_SYSTEM_INCLUDES})
  target_compile_options(iris PUBLIC /MTd)
  target_link_options(iris PUBLIC /subsystem:windows /ENTRY:mainCRTStartup)
  target_link_libraries(iris PRIVATE DirectX-Headers)

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

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/iris
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

write_basic_package_version_file(
  iris-version.cmake
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  EXPORT iris-targets
  FILE iris-targets.cmake
  NAMESPACE iris::
  DESTINATION lib/cmake/iris)

message(DEBUG "${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_LIBDIR}")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/iris-config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/iris-version.cmake"
               ${PROJECT_BINARY_DIR}/src/iris_export.h
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)
