SET(SRCS
    "hello.cpp"
)

ADD_LIBRARY(engine STATIC ${SRCS})

INCLUDE_DIRECTORIES(engine PUBLIC "${CMAKE_SOURCE_DIR}/include/engine")

