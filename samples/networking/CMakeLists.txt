set(CLIENT_SRCS
    client.cpp
)

set(SERVER_SRCS
    server.cpp
)

if(IRIS_PLATFORM MATCHES "MACOS")
    # macos specific linking
    add_executable(client ${CLIENT_SRCS})
    target_link_libraries(client "-framework AppKit")
    target_link_libraries(client "-framework CoreFoundation")

    add_executable(server ${SERVER_SRCS})
    target_link_libraries(server "-framework AppKit")
    target_link_libraries(server "-framework CoreFoundation")
elseif(IRIS_PLATFORM MATCHES "IOS")
    set(IOS_RESOURCES "Default-568h@2x.png")

    add_executable(client MACOSX_BUNDLE ${CLIENT_SRCS} ${IOS_RESOURCES})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.client")

    target_link_libraries(client "-framework UIKit")
    target_link_libraries(client "-framework Foundation")

    set_target_properties(
        client
        PROPERTIES
        MACOSX_BUNDLE YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
        RESOURCE "${IOS_RESOURCES}"
    )

endif()

if(IRIS_GRAPHICS_API MATCHES "METAL")
    target_link_libraries(client "-framework Metal")
    target_link_libraries(client "-framework MetalKit")
    target_link_libraries(client "-framework MetalPerformanceShaders")
    target_link_libraries(client "-framework QuartzCore")
    target_link_libraries(client "-framework CoreGraphics")

    target_link_libraries(server "-framework Metal")
    target_link_libraries(server "-framework MetalKit")
    target_link_libraries(server "-framework MetalPerformanceShaders")
    target_link_libraries(server "-framework QuartzCore")
    target_link_libraries(server "-framework CoreGraphics")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(client ${OPENGL_LIBRARIES})
    target_link_libraries(client "-framework CoreGraphics")

    target_link_libraries(server ${OPENGL_LIBRARIES})
    target_link_libraries(server "-framework CoreGraphics")
endif()

target_include_directories(client PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(client iris)
target_compile_options(client PRIVATE -Wall -Werror -pedantic -glldb -fno-rtti -fobjc-arc)

target_include_directories(server PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_link_libraries(server iris)
target_compile_options(server PRIVATE -Wall -Werror -pedantic -glldb -fno-rtti -fobjc-arc)
