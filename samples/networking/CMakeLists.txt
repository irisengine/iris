set(CLIENT_SRCS client.cpp)

set(SERVER_SRCS server.cpp)

if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-568h@2x.png")

  add_executable(client MACOSX_BUNDLE ${CLIENT_SRCS} ${IOS_RESOURCES})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.client")

  set_target_properties(
    client
    PROPERTIES MACOSX_BUNDLE YES
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               RESOURCE "${IOS_RESOURCES}")

else()
  add_executable(client ${CLIENT_SRCS})
endif()

add_executable(server ${SERVER_SRCS})

target_link_libraries(client iris)
target_link_libraries(server iris)
