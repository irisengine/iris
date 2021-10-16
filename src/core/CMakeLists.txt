set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/core")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("macos")
elseif(IRIS_PLATFORM MATCHES "IOS")
  add_subdirectory("ios")
elseif(IRIS_PLATFORM MATCHES "WIN32")
  add_subdirectory("win32")
endif()

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/auto_release.h
    ${INCLUDE_ROOT}/camera.h
    ${INCLUDE_ROOT}/camera_type.h
    ${INCLUDE_ROOT}/colour.h
    ${INCLUDE_ROOT}/data_buffer.h
    ${INCLUDE_ROOT}/error_handling.h
    ${INCLUDE_ROOT}/exception.h
    ${INCLUDE_ROOT}/looper.h
    ${INCLUDE_ROOT}/matrix4.h
    ${INCLUDE_ROOT}/quaternion.h
    ${INCLUDE_ROOT}/random.h
    ${INCLUDE_ROOT}/resource_loader.h
    ${INCLUDE_ROOT}/root.h
    ${INCLUDE_ROOT}/start.h
    ${INCLUDE_ROOT}/static_buffer.h
    ${INCLUDE_ROOT}/thread.h
    ${INCLUDE_ROOT}/transform.h
    ${INCLUDE_ROOT}/utils.h
    ${INCLUDE_ROOT}/vector3.h
    camera.cpp
    exception.cpp
    looper.cpp
    random.cpp
    root.cpp
    transform.cpp
    utils.cpp)
