add_executable(jobs main.cpp)

target_include_directories(jobs PRIVATE ${stb_SOURCE_DIR})

target_link_libraries(jobs iris)

if(IRIS_PLATFORM MATCHES "WINDOWS")
  set_target_properties(jobs PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

