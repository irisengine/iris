SET(SRCS
    "camera.cpp"
    "event_dispatcher.cpp"
    "gl/buffer.cpp"
    "gl/material.cpp"
    "gl/mesh_implementation.cpp"
    "gl/opengl.cpp"
    "gl/shader.cpp"
    "gl/vertex_state.cpp"
    "matrix.cpp"
    "mesh.cpp"
    "opengl_render_system.cpp"
    "vector3.cpp"
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
INCLUDE_DIRECTORIES(engine SYSTEM "${CMAKE_SOURCE_DIR}/third_party/assimp/include")
