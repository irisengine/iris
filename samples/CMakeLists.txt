add_subdirectory("animation")
add_subdirectory("cubes")
add_subdirectory("jobs")
add_subdirectory("networking")
add_subdirectory("physics")
add_subdirectory("render_graph")
add_subdirectory("sprites")

if(NOT IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("text_rendering")
endif()

