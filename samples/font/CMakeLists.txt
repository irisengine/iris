set(SAMPLE_SRCS
    main.cpp
)

add_executable(font ${SAMPLE_SRCS})
target_include_directories(font PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(font iris)

IF(PLATFORM MATCHES "OSX")
    # osx specific linking
    TARGET_LINK_LIBRARIES(font "-framework AppKit")
    TARGET_LINK_LIBRARIES(font "-framework CoreFoundation")
    IF(GRAPHICS_API MATCHES "METAL")
        TARGET_LINK_LIBRARIES(font "-framework Metal")
        TARGET_LINK_LIBRARIES(font "-framework MetalKit")
        TARGET_LINK_LIBRARIES(font "-framework QuartzCore")
    ELSE()
        FIND_PACKAGE(OpenGL REQUIRED)
        INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
        TARGET_LINK_LIBRARIES(font ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(font "-framework CoreGraphics")
    ENDIF()
ENDIF()
