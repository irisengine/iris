add_executable(sclera
  main.cpp
)

target_link_libraries(sclera iris)

if(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(sclera PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()