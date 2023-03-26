FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG 5a2b1e84828f192d30c91cdd210db41de8ad3236
  CONFIGURE_COMMAND "" BUILD_COMMAND "")
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  imgui_gizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo
  GIT_TAG 1.83
  CONFIGURE_COMMAND "" BUILD_COMMAND "")
FetchContent_MakeAvailable(imgui_gizmo)

add_executable(sclera
  ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_gizmo_SOURCE_DIR}/ImGuizmo.cpp
  gui.cpp
  main.cpp
  metal_gui.mm
  metal_gui_renderer.mm
)

target_include_directories(sclera SYSTEM
  PRIVATE ${imgui_SOURCE_DIR} ${imgui_gizmo_SOURCE_DIR})

target_link_libraries(sclera iris)
target_compile_definitions(sclera PRIVATE "IMGUI_DEFINE_MATH_OPERATORS=1")

if(IRIS_PLATFORM MATCHES "MACOS")
  target_compile_options(sclera PRIVATE -Wall -Werror -pedantic -glldb -fobjc-arc)
elseif(IRIS_PLATFORM MATCHES "WIN32")
  set_target_properties(sclera PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  target_compile_options(sclera PRIVATE /W4 /WX)
endif()