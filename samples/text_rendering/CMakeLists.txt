set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(text_rendering ${SAMPLE_SRCS})
    target_link_libraries(text_rendering "-framework AppKit")
    target_link_libraries(text_rendering "-framework CoreFoundation")
    target_link_libraries(text_rendering "-framework CoreText")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(text_rendering MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.text_rendering")

    target_link_libraries(text_rendering "-framework UIKit")
    target_link_libraries(text_rendering "-framework CoreFoundation")
    target_link_libraries(text_rendering "-framework Foundation")
    target_link_libraries(text_rendering "-framework CoreText")

    set_target_properties(
        text_rendering
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(text_rendering "-framework Metal")
    target_link_libraries(text_rendering "-framework MetalKit")
    target_link_libraries(text_rendering "-framework MetalPerformanceShaders")
    target_link_libraries(text_rendering "-framework QuartzCore")
    target_link_libraries(text_rendering "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(text_rendering ${OPENGL_LIBRARIES})
    target_link_libraries(text_rendering "-framework CoreGraphics")
endif()

target_include_directories(text_rendering PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(text_rendering iris)

