set(SAMPLE_SRCS
    main.cpp
)

add_executable(font ${SAMPLE_SRCS})
target_include_directories(font PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(font iris)

if(PLATFORM MATCHES "MACOS")
    # macos specific linking
    target_link_libraries(font "-framework AppKit")
    target_link_libraries(font "-framework CoreFoundation")
    if(GRAPHICS_API MATCHES "METAL")
        target_link_libraries(font "-framework Metal")
        target_link_libraries(font "-framework MetalKit")
        target_link_libraries(font "-framework QuartzCore")
    else()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        target_link_libraries(font ${OPENGL_LIBRARIES})
        target_link_libraries(font "-framework CoreGraphics")
    endif()
endif()
