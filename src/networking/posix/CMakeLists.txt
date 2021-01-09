set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/networking/posix")

set(NETWORKING_SRCS
    "${SOURCE_ROOT}/simulated_server_socket.cpp"
    "${SOURCE_ROOT}/simulated_socket.cpp" "${SOURCE_ROOT}/udp_server_socket.cpp"
    "${SOURCE_ROOT}/udp_socket.cpp"
    PARENT_SCOPE)
