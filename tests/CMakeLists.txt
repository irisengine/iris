add_subdirectory(third_party/googletest)

if(PLATFORM MATCHES "MACOS")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/platform/macos")
    add_executable(unit_tests ${PLATFORM_TEST_SRCS} ${MACOS_PLATFORM_TEST_SRCS})
endif()

target_link_libraries(unit_tests iris)
target_link_libraries(unit_tests gtest_main)

IF(PLATFORM MATCHES "MACOS")
    # macos specific linking
    target_link_libraries(unit_tests "-framework AppKit")
    target_link_libraries(unit_tests "-framework CoreFoundation")
    if(GRAPHICS_API MATCHES "METAL")
        target_link_libraries(unit_tests "-framework Metal")
        target_link_libraries(unit_tests "-framework MetalKit")
    elseif()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        target_link_libraries(unit_tests ${OPENGL_LIBRARIES})
        target_link_libraries(unit_tests "-framework CoreGraphics")
    endif()
endif()

include_directories(unit_tests SYSTEM third_party/googletest/googletest/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/tests)

