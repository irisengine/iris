if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-568h@2x.png")

  add_executable(window MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.window")

  set_target_properties(
    window
    PROPERTIES MACOSX_BUNDLE YES
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               RESOURCE "${IOS_RESOURCES}")
else()
  add_executable(window main.cpp)
endif()

target_include_directories(window PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(window PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_link_libraries(window iris)

if(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(window PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
