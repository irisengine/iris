add_subdirectory("core")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("log")
add_subdirectory("networking")
add_subdirectory("physics")
add_subdirectory("platform")

add_library(
  iris STATIC
  ${CORE_SRCS}
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

target_include_directories(iris SYSTEM
                           PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
target_include_directories(iris SYSTEM
                           PRIVATE "${CMAKE_SOURCE_DIR}/third_party/bullet/src")
target_include_directories(
  iris SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party/assimp/include")
target_include_directories(
  iris SYSTEM PRIVATE "${CMAKE_BINARY_DIR}/third_party/assimp/include")

set(LINK_FRAMEWORKS "")

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
  target_compile_definitions(iris PRIVATE PLATFORM_MACOS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_X86_64)
  set(IRIS_ARCH "X86_64")
  if(NOT IRIS_GRAPHICS_API)
    set(IRIS_GRAPHICS_API "METAL")
  endif()
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "FIBERS")
  endif()
elseif(IRIS_PLATFORM MATCHES "IOS")
  target_compile_definitions(iris PRIVATE PLATFORM_IOS)
  target_compile_definitions(iris PRIVATE IRIS_ARCH_ARM64)
  set(IRIS_ARCH "ARM64")
  if(NOT IRIS_GRAPHICS_API)
    set(IRIS_GRAPHICS_API "METAL")
  endif()
  if(NOT IRIS_JOBS_API)
    set(IRIS_JOBS_API "THREADS")
  endif()
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
  if(IRIS_PLATFORM MATCHES "MACOS")
    find_package(OpenGL REQUIRED)
    target_include_directories(iris SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
    set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenGL")
    target_compile_definitions(iris PRIVATE IRIS_GRAPHICS_API_OPENGL)
    # opengl is technically deprecated on macos, so silence the deprecation
    # warnings
    target_compile_definitions(iris PRIVATE GL_SILENCE_DEPRECATION)
  else()
    message(FATAL_ERROR "OPENGL not supported on ${IRIS_PLATFORM}")
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
target_compile_options(iris PRIVATE -Wall -Werror -pedantic -glldb -fno-rtti
                                    -fobjc-arc)

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
