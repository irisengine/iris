set(SAMPLE_SRCS main.cpp)

if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-568h@2x.png" "circle.png")

  add_executable(sprites MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.sprites")

  set_target_properties(
    sprites
    PROPERTIES MACOSX_BUNDLE YES
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               RESOURCE "${IOS_RESOURCES}")

  set_source_files_properties(${IOS_RESOURCES}
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
else()
  add_executable(sprites ${SAMPLE_SRCS})
endif()

file(COPY "circle.png" DESTINATION ${CMAKE_BINARY_DIR}/samples/sprites)

target_link_libraries(sprites iris)
