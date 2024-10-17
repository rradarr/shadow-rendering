# set(FINAL_DIR $<$<CONFIG:Debug>:Debug>)
# $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
# file(COPY ../Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

# file(GLOB MY_SHADERS
#   "Shaders/*.hlsl"
# )
# file(COPY ${MY_SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Resources)

add_executable(${CMAKE_PROJECT_NAME} WIN32
main.cpp
stdafx.cpp
BufferMemoryManager.cpp
Camera.cpp
CbvSrvDescriptorHeapManager.cpp
ConsoleHelper.cpp
DefaultRenderer.cpp
DefaultTexturedMaterial.cpp
DXContext.cpp
Engine.cpp
EngineHelpers.cpp
EngineStateModel.cpp
ImGuiController.cpp
ImGuiPerformanceWindow.cpp
ImGuiRenderingOptions.cpp
LitMaterial.cpp
MainInputController.cpp
Material.cpp
Mesh.cpp
NormalsDebugMaterial.cpp
NormalsDebugRenderer.cpp
ProjectionShadowMaterial.cpp
ProjectionShadowRenderer.cpp
Renderer.cpp
ResourceManager.cpp
RollingAverageUtil.cpp
SceneObject.cpp
ShadowMapDepthMaterial.cpp
ShadowMapMainMaterial.cpp
ShadowMapRenderer.cpp
StandardMaterial.cpp
Texture.cpp
TextureLoader.cpp
Timer.cpp
VoyagerEngine.cpp
WindowsApplication.cpp
WireframeMaterial.cpp
WireframeRenderer.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui)

add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/Shaders
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Resources)

add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Resources)

