set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/log")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/basic_formatter.h
    ${INCLUDE_ROOT}/colour_formatter.h
    ${INCLUDE_ROOT}/emoji_formatter.h
    ${INCLUDE_ROOT}/file_outputter.h
    ${INCLUDE_ROOT}/log_level.h
    ${INCLUDE_ROOT}/log.h
    ${INCLUDE_ROOT}/logger.h
    ${INCLUDE_ROOT}/stdout_outputter.h
    basic_formatter.cpp
    colour_formatter.cpp
    emoji_formatter.cpp
    file_outputter.cpp
    stdout_outputter.cpp)
