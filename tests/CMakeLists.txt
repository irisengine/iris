
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

mark_as_advanced(BUILD_GMOCK BUILD_GTEST gtest_hide_internal_symbols)

add_executable(unit_tests "")

add_subdirectory("core")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("networking")
add_subdirectory("platform")

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(unit_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(gmock_main PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

target_link_libraries(unit_tests iris gmock_main)
target_compile_definitions(unit_tests PRIVATE IRIS_FORCE_EXPECT)
gtest_discover_tests(unit_tests)
