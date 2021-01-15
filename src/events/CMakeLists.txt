set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/events")

set(EVENTS_SRCS
    ${EVENTS_SRCS}
    "${INCLUDE_ROOT}/keyboard_event.h"
    "${INCLUDE_ROOT}/mouse_event.h"
    "${INCLUDE_ROOT}/quit_event.h"
    "${INCLUDE_ROOT}/touch_event.h"
    "${SOURCE_ROOT}/event.cpp"
    "${INCLUDE_ROOT}/event.h"
    PARENT_SCOPE)
