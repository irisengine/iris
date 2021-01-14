add_subdirectory("core")
add_subdirectory("events")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("log")
add_subdirectory("networking")
add_subdirectory("physics")
add_subdirectory("platform")

add_library(
  iris STATIC
  ${CORE_SRCS}
  ${EVENTS_SRCS}
  ${GRAPHICS_SRCS}
  ${JOB_SRCS}
  ${LOG_SRCS}
  ${NETWORKING_SRCS}
  ${PHYSICS_SRCS}
  ${PLATFORM_SRCS})

add_library(iris::iris ALIAS iris)

target_include_directories(
  iris PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/iris>
              $<INSTALL_INTERFACE:include/iris>)

# on windows SYSTEM includes do not prevent warnings - so we add bullet using
# the /external flag
if(IRIS_PLATFORM MATCHES "WINDOWS")
  set(WINDOWS_SYSTEM_INCLUDES /external:I
                              ${CMAKE_SOURCE_DIR}/third_party/bullet/src)
else()
  target_include_directories(
    iris SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party/bullet/src")
  target_include_directories(iris SYSTEM
                             PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
endif()

target_include_directories(iris SYSTEM
                           PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
target_include_directories(iris SYSTEM
                           PRIVATE "${CMAKE_SOURCE_DIR}/third_party/assimp")

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
  set(IRIS_ARCH "X86_64")
  if(NOT IRIS_GRAPHICS_API)
    set(IRIS_GRAPHICS_API "METAL")
  endif()
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "FIBERS")
  endif()
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_IOS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_ARM64)
  set(IRIS_ARCH "ARM64")
  if(NOT IRIS_GRAPHICS_API)
    set(IRIS_GRAPHICS_API "METAL")
  endif()
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
  target_compile_definitions(iris PUBLIC IRIS_PLATFORM_WINDOWS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_X86_64)
  set(IRIS_GRAPHICS_API "OPENGL")
  set(IRIS_JOBS_API "THREADS")
  set(IRIS_ARCH "X86_64")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

# handle graphic api setup including validating they are supported on current
# platform
if(IRIS_GRAPHICS_API MATCHES "METAL")
  if(IRIS_PLATFORM MATCHES "MACOS" OR IRIS_PLATFORM MATCHES "IOS")
    set(LINK_FRAMEWORKS
        "${LINK_FRAMEWORKS} -framework Metal -framework MetalKit -framework MetalPerformanceShaders -framework QuartzCore"
    )
    target_compile_definitions(iris PRIVATE IRIS_GRAPHICS_API_METAL)
  else()
    message(FATAL_ERROR "METAL not supported on ${IRIS_PLATFORM}")
  endif()
elseif(IRIS_GRAPHICS_API MATCHES "OPENGL")
  find_package(OpenGL REQUIRED)
  target_include_directories(iris SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
  target_compile_definitions(iris PRIVATE IRIS_GRAPHICS_API_OPENGL)
  if(IRIS_PLATFORM MATCHES "MACOS")
    set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenGL")
    target_compile_definitions(iris PRIVATE IRIS_GRAPHICS_API_OPENGL)
    # opengl is technically deprecated on macos, so silence the deprecation
    # warnings
    target_compile_definitions(iris PRIVATE GL_SILENCE_DEPRECATION)
  endif()
else()
  message(FATAL_ERROR "Unsupported graphics API: ${IRIS_GRAPHICS_API}")
endif()

message(
  STATUS
    "Building for ${IRIS_PLATFORM} with ${IRIS_GRAPHICS_API} (${IRIS_JOBS_API})"
)

target_link_libraries(iris PUBLIC LinearMath BulletDynamics BulletCollision
                                  assimp ${LINK_FRAMEWORKS})

if(IRIS_PLATFORM MATCHES "WINDOWS")
  target_compile_options(iris PRIVATE /W4 /WX /experimental:external
                                      /external:W0 ${WINDOWS_SYSTEM_INCLUDES})
  target_compile_options(iris PUBLIC /MTd)
  target_link_options(iris PUBLIC /subsystem:windows /ENTRY:mainCRTStartup)
else()
  target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fobjc-arc)
endif()

install(
  TARGETS iris
          LinearMath
          BulletDynamics
          BulletCollision
          assimp
          zlibstatic
          IrrXML
  EXPORT iris-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

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
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iris)
