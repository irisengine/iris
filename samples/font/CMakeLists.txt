set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(font ${SAMPLE_SRCS})
    target_link_libraries(font "-framework AppKit")
    target_link_libraries(font "-framework CoreFoundation")
    target_link_libraries(font "-framework CoreText")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(font MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.font")

    target_link_libraries(font "-framework UIKit")
    target_link_libraries(font "-framework CoreFoundation")
    target_link_libraries(font "-framework Foundation")
    target_link_libraries(font "-framework CoreText")

    set_target_properties(
        font
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(font "-framework Metal")
    target_link_libraries(font "-framework MetalKit")
    target_link_libraries(font "-framework QuartzCore")
    target_link_libraries(font "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(font ${OPENGL_LIBRARIES})
    target_link_libraries(font "-framework CoreGraphics")
endif()

target_include_directories(font PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(font iris)

