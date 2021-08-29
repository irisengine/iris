set(SAMPLE_SRCS
    main.cpp
    samples/render_graph_sample.h
    samples/render_graph_sample.cpp
    samples/sample.h
    samples/animation_sample.h
    samples/animation_sample.cpp
    samples/physics_sample.h
    samples/physics_sample.cpp
    )

add_executable(sample_browser ${SAMPLE_SRCS})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/samples/sample_browser)

target_include_directories(sample_browser
                           PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_include_directories(
  sample_browser PUBLIC "${PROJECT_SOURCE_DIR}/samples/sample_browser")
target_link_libraries(sample_browser iris)
