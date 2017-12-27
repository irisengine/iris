SET(SRCS
    "gl/material.cpp"
    "gl/opengl.cpp"
    "gl/shader.cpp"
    "gl/vertex_data.cpp"
    "gl/vertex_state.cpp"
    "mesh.cpp"
)

IF(PLATFORM MATCHES "OSX")
    SET(PLATFORM_SRCS
        "osx/AppDelegate.m"
        "osx/OpenGLView.m"
        "osx/window.mm"
    )
ENDIF()

ADD_LIBRARY(engine STATIC ${SRCS} ${PLATFORM_SRCS})

INCLUDE_DIRECTORIES(engine PUBLIC "${CMAKE_SOURCE_DIR}/include/engine")

