set(SAMPLE_SRCS main.cpp)

if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-568h@2x.png")

  add_executable(text_rendering MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.text_rendering")

  set_target_properties(
    text_rendering
    PROPERTIES MACOSX_BUNDLE YES
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               RESOURCE "${IOS_RESOURCES}")
else()
  add_executable(text_rendering ${SAMPLE_SRCS})
endif()

target_link_libraries(text_rendering iris)
