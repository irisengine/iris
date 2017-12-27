SET(SRCS
    "game.cpp"
)

ADD_EXECUTABLE(ilis ${SRCS})

# create application and link in engine
TARGET_LINK_LIBRARIES(ilis engine)

INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/game")
INCLUDE_DIRECTORIES(ilis PUBLIC "${CMAKE_SOURCE_DIR}/include/engine")

