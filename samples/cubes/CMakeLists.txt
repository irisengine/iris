set(SAMPLE_SRCS
    main.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(cubes ${SAMPLE_SRCS})
    target_link_libraries(cubes "-framework AppKit")
    target_link_libraries(cubes "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(cubes MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.cubes")

    target_link_libraries(cubes "-framework UIKit")
    target_link_libraries(cubes "-framework Foundation")

    set_target_properties(
        cubes
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(cubes "-framework Metal")
    target_link_libraries(cubes "-framework MetalKit")
    target_link_libraries(cubes "-framework MetalPerformanceShaders")
    target_link_libraries(cubes "-framework QuartzCore")
    target_link_libraries(cubes "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(cubes ${OPENGL_LIBRARIES})
    target_link_libraries(cubes "-framework CoreGraphics")
endif()

target_include_directories(cubes PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(cubes iris)
