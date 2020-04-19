set(SAMPLE_SRCS
    main.cpp
)

add_executable(sprites ${SAMPLE_SRCS})
target_include_directories(sprites PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(sprites iris)

file(COPY "circle.png" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

if(PLATFORM MATCHES "MACOS")
    # macos specific linking
    target_link_libraries(sprites "-framework AppKit")
    target_link_libraries(sprites "-framework CoreFoundation")
    if(GRAPHICS_API MATCHES "METAL")
        target_link_libraries(sprites "-framework Metal")
        target_link_libraries(sprites "-framework MetalKit")
        target_link_libraries(sprites "-framework QuartzCore")
    else()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        target_link_libraries(sprites ${OPENGL_LIBRARIES})
        target_link_libraries(sprites "-framework CoreGraphics")
    endif()
endif()
