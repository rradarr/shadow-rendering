file(COPY ../Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(GLOB MY_SHADERS
  "Shaders/*.hlsl"
)
file(COPY ${MY_SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Resources)

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
ImGuiController.cpp
SceneObject.cpp
LitMaterial.cpp
Material.cpp
Mesh.cpp
NormalsDebugMaterial.cpp
Renderer.cpp
ResourceManager.cpp
StandardMaterial.cpp
Texture.cpp
TextureLoader.cpp
Timer.cpp
VoyagerEngine.cpp
WindowsApplication.cpp
WireframeMaterial.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui)