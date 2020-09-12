set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(window ${SAMPLE_SRCS})
    target_link_libraries(window "-framework AppKit")
    target_link_libraries(window "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(window MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.window")

    target_link_libraries(window "-framework UIKit")
    target_link_libraries(window "-framework Foundation")

    set_target_properties(
        window
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(window "-framework Metal")
    target_link_libraries(window "-framework MetalKit")
    target_link_libraries(window "-framework QuartzCore")
    target_link_libraries(window "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(window ${OPENGL_LIBRARIES})
    target_link_libraries(window "-framework CoreGraphics")
endif()

target_include_directories(window PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(window iris)
