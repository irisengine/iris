SET(SRCS
    "game.cpp"
)

ADD_EXECUTABLE(ilis ${SRCS})

# create application and link in engine
TARGET_LINK_LIBRARIES(ilis engine)

IF(PLATFORM MATCHES "OSX")
    # osx specific linking
    FIND_PACKAGE(OpenGL REQUIRED)
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(ilis ${OPENGL_LIBRARIES})
    TARGET_LINK_LIBRARIES(ilis "-framework AppKit")
    TARGET_LINK_LIBRARIES(ilis "-framework CoreFoundation")
    TARGET_LINK_LIBRARIES(ilis "-framework CoreGraphics")
    TARGET_LINK_LIBRARIES(ilis "-framework CoreText")
ENDIF()

INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/core")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/game")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/graphics")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/physics")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/platform")

