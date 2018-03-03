SET(SRCS
    "box.cpp"
    "camera.cpp"
    "collision_detector.cpp"
    "collision_resolver.cpp"
    "contact.cpp"
    "entity.cpp"
    "event_dispatcher.cpp"
    "gl/buffer.cpp"
    "gl/material_implementation.cpp"
    "gl/mesh_implementation.cpp"
    "gl/opengl.cpp"
    "gl/render_system.cpp"
    "gl/shader.cpp"
    "gl/texture_implementation.cpp"
    "gl/vertex_state.cpp"
    "material.cpp"
    "matrix3.cpp"
    "matrix4.cpp"
    "mesh.cpp"
    "physics_system.cpp"
    "plane.cpp"
    "quaternion.cpp"
    "rigid_body.cpp"
    "texture.cpp"
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
INCLUDE_DIRECTORIES(engine SYSTEM "${CMAKE_SOURCE_DIR}/third_party")
INCLUDE_DIRECTORIES(engine SYSTEM "${CMAKE_SOURCE_DIR}/third_party/assimp/include")
