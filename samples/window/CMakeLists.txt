add_executable(window main.cpp)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/samples/window)

target_include_directories(window PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(window PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_link_libraries(window iris)

if(IRIS_PLATFORM MATCHES "WINDOWS")
  set_target_properties(window PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
