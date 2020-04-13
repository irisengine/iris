set(SAMPLE_SRCS
    main.cpp
)

add_executable(sprites ${SAMPLE_SRCS})
target_include_directories(sprites PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(sprites iris)

IF(PLATFORM MATCHES "OSX")
    # osx specific linking
    TARGET_LINK_LIBRARIES(sprites "-framework AppKit")
    TARGET_LINK_LIBRARIES(sprites "-framework CoreFoundation")
    IF(GRAPHICS_API MATCHES "METAL")
        TARGET_LINK_LIBRARIES(sprites "-framework Metal")
        TARGET_LINK_LIBRARIES(sprites "-framework MetalKit")
        TARGET_LINK_LIBRARIES(sprites "-framework QuartzCore")
    ELSE()
        FIND_PACKAGE(OpenGL REQUIRED)
        INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
        TARGET_LINK_LIBRARIES(sprites ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(sprites "-framework CoreGraphics")
    ENDIF()
ENDIF()
