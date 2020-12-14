set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(render_graph ${SAMPLE_SRCS})
    target_link_libraries(render_graph "-framework AppKit")
    target_link_libraries(render_graph "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(render_graph MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.render_graph")

    target_link_libraries(render_graph "-framework UIKit")
    target_link_libraries(render_graph "-framework Foundation")

    set_target_properties(
        render_graph
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(render_graph "-framework Metal")
    target_link_libraries(render_graph "-framework MetalKit")
    target_link_libraries(render_graph "-framework MetalPerformanceShaders")
    target_link_libraries(render_graph "-framework QuartzCore")
    target_link_libraries(render_graph "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(render_graph ${OPENGL_LIBRARIES})
    target_link_libraries(render_graph "-framework CoreGraphics")
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/samples/render_graph)

target_include_directories(render_graph PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(render_graph iris)
