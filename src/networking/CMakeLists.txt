set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/networking")

add_subdirectory("posix")

if(IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("windows")
endif()

set(NETWORKING_SRCS
    ${NETWORKING_SRCS}
    ${PLATFORM_SRCS}
    "${INCLUDE_ROOT}/accepting_socket.h"
    "${INCLUDE_ROOT}/channel/channel_type.h"
    "${INCLUDE_ROOT}/data_buffer_deserialiser.h"
    "${INCLUDE_ROOT}/data_buffer_serialiser.h"
    "${INCLUDE_ROOT}/packet_type.h"
    "${INCLUDE_ROOT}/simulated_accepting_socket.h"
    "${INCLUDE_ROOT}/simulated_socket.h"
    "${INCLUDE_ROOT}/socket.h"
    "${INCLUDE_ROOT}/udp_accepting_socket.h"
    "${INCLUDE_ROOT}/udp_socket.h"
    "${SOURCE_ROOT}/channel/channel.cpp"
    "${INCLUDE_ROOT}/channel/channel.h"
    "${SOURCE_ROOT}/channel/reliable_ordered_channel.cpp"
    "${INCLUDE_ROOT}/channel/reliable_ordered_channel.h"
    "${SOURCE_ROOT}/channel/unreliable_sequenced_channel.cpp"
    "${INCLUDE_ROOT}/channel/unreliable_sequenced_channel.h"
    "${SOURCE_ROOT}/channel/unreliable_unordered_channel.cpp"
    "${INCLUDE_ROOT}/channel/unreliable_unordered_channel.h"
    "${SOURCE_ROOT}/client_connection_handler.cpp"
    "${INCLUDE_ROOT}/client_connection_handler.h"
    "${SOURCE_ROOT}/packet.cpp"
    "${INCLUDE_ROOT}/packet.h"
    "${SOURCE_ROOT}/server_connection_handler.cpp"
    "${INCLUDE_ROOT}/server_connection_handler.h"
    PARENT_SCOPE)
