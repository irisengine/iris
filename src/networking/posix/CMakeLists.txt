set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/networking/posix")

set(NETWORKING_SRCS
    "${INCLUDE_ROOT}/auto_socket.h"
    "${INCLUDE_ROOT}/client_connection.h" "${SOURCE_ROOT}/client_connection.cpp"
    "${SOURCE_ROOT}/simulated_accepting_socket.cpp"
    "${SOURCE_ROOT}/simulated_socket.cpp"
    "${SOURCE_ROOT}/udp_accepting_socket.cpp"
    "${SOURCE_ROOT}/udp_socket.cpp"

    PARENT_SCOPE
)
