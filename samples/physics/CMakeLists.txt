set(SAMPLE_SRCS main.cpp)

if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-828h@3x.png" "Default-Landscape-828h@3x.png")

  add_executable(physics ${SAMPLE_SRCS} ${IOS_RESOURCES})

  set_target_properties(
    physics
    PROPERTIES MACOSX_BUNDLE YES
               MACOSX_BUNDLE_INFO_PLIST
               "${CMAKE_CURRENT_SOURCE_DIR}/template_info.plist"
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER
               "${IRIS_BUNDLE_IDENTIFIER}.physics"
               RESOURCE "${IOS_RESOURCES}")

else()
  add_executable(physics ${SAMPLE_SRCS})
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/samples/physics)

target_link_libraries(physics iris)
