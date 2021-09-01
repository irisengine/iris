set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/networking")

if(IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("windows")
endif()

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/channel/channel.h
    ${INCLUDE_ROOT}/channel/channel_type.h
    ${INCLUDE_ROOT}/channel/reliable_ordered_channel.h
    ${INCLUDE_ROOT}/channel/unreliable_sequenced_channel.h
    ${INCLUDE_ROOT}/channel/unreliable_unordered_channel.h
    ${INCLUDE_ROOT}/client_connection_handler.h
    ${INCLUDE_ROOT}/data_buffer_deserialiser.h
    ${INCLUDE_ROOT}/data_buffer_serialiser.h
    ${INCLUDE_ROOT}/networking.h
    ${INCLUDE_ROOT}/packet.h
    ${INCLUDE_ROOT}/packet_type.h
    ${INCLUDE_ROOT}/server_connection_handler.h
    ${INCLUDE_ROOT}/server_socket.h
    ${INCLUDE_ROOT}/simulated_server_socket.h
    ${INCLUDE_ROOT}/simulated_socket.h
    ${INCLUDE_ROOT}/socket.h
    ${INCLUDE_ROOT}/udp_server_socket.h
    ${INCLUDE_ROOT}/udp_socket.h
    channel/channel.cpp
    channel/reliable_ordered_channel.cpp
    channel/unreliable_sequenced_channel.cpp
    channel/unreliable_unordered_channel.cpp
    client_connection_handler.cpp
    packet.cpp
    server_connection_handler.cpp
    simulated_server_socket.cpp
    simulated_socket.cpp
    udp_server_socket.cpp
    udp_socket.cpp)
