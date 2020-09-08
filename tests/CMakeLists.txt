add_subdirectory(third_party/googletest)

add_subdirectory("core")
add_subdirectory("jobs")
add_subdirectory("platform")
add_subdirectory("networking")

add_executable(unit_tests
    ${CORE_TEST_SRCS}
    ${JOBS_TEST_SRCS}
    ${PLATFORM_TEST_SRCS}
    ${NETWORKING_TEST_SRCS}
)

target_link_libraries(unit_tests iris)
target_link_libraries(unit_tests gmock_main)
target_compile_options(unit_tests PRIVATE -glldb -fno-rtti -fobjc-arc)

include_directories(unit_tests SYSTEM third_party/googletest/googletest/include)
include_directories(unit_tests SYSTEM third_party/googletest/googlemock/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/tests)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    target_link_libraries(unit_tests "-framework AppKit")
    target_link_libraries(unit_tests "-framework CoreFoundation")
endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(unit_tests "-framework Metal")
    target_link_libraries(unit_tests "-framework MetalKit")
    target_link_libraries(unit_tests "-framework QuartzCore")
    target_link_libraries(unit_tests "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(unit_tests ${OPENGL_LIBRARIES})
    target_link_libraries(unit_tests "-framework CoreGraphics")
endif()

