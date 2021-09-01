set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/events")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/event.h
    ${INCLUDE_ROOT}/event_type.h
    ${INCLUDE_ROOT}/keyboard_event.h
    ${INCLUDE_ROOT}/mouse_event.h
    ${INCLUDE_ROOT}/quit_event.h
    ${INCLUDE_ROOT}/touch_event.h
    event.cpp)
