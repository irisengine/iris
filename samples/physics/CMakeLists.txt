set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(physics ${SAMPLE_SRCS})
    target_link_libraries(physics "-framework AppKit")
    target_link_libraries(physics "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES
        "Default-828h@3x.png"
        "Default-Landscape-828h@3x.png"
     )

    add_executable(physics ${SAMPLE_SRCS} ${IOS_RESOURCES})

    target_link_libraries(physics "-framework UIKit")
    target_link_libraries(physics "-framework Foundation")

    set_target_properties(
        physics
        PROPERTIES
        MACOSX_BUNDLE YES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/template_info.plist"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.physics"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(physics "-framework Metal")
    target_link_libraries(physics "-framework MetalKit")
    target_link_libraries(physics "-framework QuartzCore")
    target_link_libraries(physics "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(physics ${OPENGL_LIBRARIES})
    target_link_libraries(physics "-framework CoreGraphics")
endif()

target_include_directories(physics PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(physics iris)
