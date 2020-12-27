enable_language(C ASM)

set(SAMPLE_SRCS main.cpp)

if(IRIS_PLATFORM MATCHES "IOS")
  set(IOS_RESOURCES "Default-568h@2x.png")

  add_executable(jobs MACOSX_BUNDLE ${SAMPLE_SRCS} ${IOS_RESOURCES})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${IRIS_BUNDLE_IDENTIFIER}.jobs")

  set_target_properties(
    jobs
    PROPERTIES MACOSX_BUNDLE YES
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${IRIS_DEVELOPMENT_TEAM}"
               RESOURCE "${IOS_RESOURCES}")
else()
  add_executable(jobs ${SAMPLE_SRCS})
endif()

target_include_directories(jobs PUBLIC "${CMAKE_SOURCE_DIR}/include/iris")
target_include_directories(jobs PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_link_libraries(jobs iris)
