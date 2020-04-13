set(SAMPLE_SRCS
    main.cpp
)

add_executable(window ${SAMPLE_SRCS})
target_include_directories(window PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(window iris)

IF(PLATFORM MATCHES "OSX")
    # osx specific linking
    TARGET_LINK_LIBRARIES(window "-framework AppKit")
    TARGET_LINK_LIBRARIES(window "-framework CoreFoundation")
    IF(GRAPHICS_API MATCHES "METAL")
        TARGET_LINK_LIBRARIES(window "-framework Metal")
        TARGET_LINK_LIBRARIES(window "-framework MetalKit")
        TARGET_LINK_LIBRARIES(window "-framework QuartzCore")
    ELSE()
        FIND_PACKAGE(OpenGL REQUIRED)
        INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
        TARGET_LINK_LIBRARIES(window ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(window "-framework CoreGraphics")
    ENDIF()
ENDIF()
