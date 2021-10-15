add_executable(window main.cpp)

target_include_directories(window PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(window PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_link_libraries(window iris)

if(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(window PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
