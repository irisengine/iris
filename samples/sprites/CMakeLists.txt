set(SAMPLE_SRCS
    main.cpp
)

file(COPY "circle.png" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(sprites ${SAMPLE_SRCS})
    target_link_libraries(sprites "-framework AppKit")
    target_link_libraries(sprites "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png" "circle.png")

    add_executable(sprites MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.sprites")

    target_link_libraries(sprites "-framework UIKit")
    target_link_libraries(sprites "-framework Foundation")
    target_link_libraries(sprites "-framework CoreFoundation")

    set_target_properties(
        sprites
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )
    
    set_source_files_properties(${IOS_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(sprites "-framework Metal")
    target_link_libraries(sprites "-framework MetalKit")
    target_link_libraries(sprites "-framework QuartzCore")
    target_link_libraries(sprites "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(sprites ${OPENGL_LIBRARIES})
    target_link_libraries(sprites "-framework CoreGraphics")
endif()

target_include_directories(sprites PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(sprites iris)
