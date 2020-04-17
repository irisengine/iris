set(SAMPLE_SRCS
    main.cpp
)

add_executable(window ${SAMPLE_SRCS})
target_include_directories(window PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(window iris)

if(PLATFORM MATCHES "OSX")
    # osx specific linking
    target_link_libraries(window "-framework AppKit")
    target_link_libraries(window "-framework CoreFoundation")
    if(GRAPHICS_API MATCHES "METAL")
        target_link_libraries(window "-framework Metal")
        target_link_libraries(window "-framework MetalKit")
        target_link_libraries(window "-framework QuartzCore")
    else()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        target_link_libraries(window ${OPENGL_LIBRARIES})
        target_link_libraries(window "-framework CoreGraphics")
    endif()
endif()
