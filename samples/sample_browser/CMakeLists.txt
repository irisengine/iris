add_executable(sample_browser
    main.cpp
    samples/animation_sample.cpp
    samples/animation_sample.h
    samples/physics_sample.cpp
    samples/physics_sample.h
    samples/render_graph_sample.cpp
    samples/render_graph_sample.h
    samples/sample.h)

file(COPY assets DESTINATION ${PROJECT_BINARY_DIR}/samples/sample_browser)

target_include_directories(sample_browser
                           PRIVATE ${stb_SOURCE_DIR})
target_include_directories(
  sample_browser PUBLIC "${PROJECT_SOURCE_DIR}/samples/sample_browser")
target_link_libraries(sample_browser iris)

if(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(sample_browser PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
