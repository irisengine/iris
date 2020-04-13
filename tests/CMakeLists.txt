ADD_SUBDIRECTORY(third_party/googletest)

IF(PLATFORM MATCHES "OSX")
    ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/platform/osx")
    ADD_EXECUTABLE(unit_tests ${PLATFORM_TEST_SRCS} ${OSX_PLATFORM_TEST_SRCS})
ENDIF()

TARGET_LINK_LIBRARIES(unit_tests iris)
TARGET_LINK_LIBRARIES(unit_tests gtest_main)

IF(PLATFORM MATCHES "OSX")
    # osx specific linking
    TARGET_LINK_LIBRARIES(unit_tests "-framework AppKit")
    TARGET_LINK_LIBRARIES(unit_tests "-framework CoreFoundation")
    IF(GRAPHICS_API MATCHES "METAL")
        TARGET_LINK_LIBRARIES(unit_tests "-framework Metal")
        TARGET_LINK_LIBRARIES(unit_tests "-framework MetalKit")
    ELSEIF()
        FIND_PACKAGE(OpenGL REQUIRED)
        INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
        TARGET_LINK_LIBRARIES(unit_tests ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(unit_tests "-framework CoreGraphics")
    ENDIF()
ENDIF()

INCLUDE_DIRECTORIES(unit_tests SYSTEM third_party/googletest/googletest/include)
INCLUDE_DIRECTORIES(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include/platform)
INCLUDE_DIRECTORIES(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/tests)

