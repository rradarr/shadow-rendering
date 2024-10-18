cmake_minimum_required(VERSION 3.8.0)
project(CmakeTestProject VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Treat compiler warnings as errors.
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(FetchContent)
FetchContent_Declare(
    dx_headers
    GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
    GIT_TAG v1.614.0
)
FetchContent_MakeAvailable(dx_headers)

FetchContent_Declare(
    dx_residency_manager
    URL https://raw.githubusercontent.com/microsoft/DirectX-Graphics-Samples/master/Libraries/D3DX12Residency/d3dx12Residency.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(dx_residency_manager)

FetchContent_Declare(
    dx_tool_kit
    URL https://github.com/microsoft/DirectXTK12/archive/refs/tags/jun2024.zip
)
FetchContent_MakeAvailable(dx_tool_kit)

FetchContent_Declare(
    dx_texture_kit
    URL https://github.com/microsoft/DirectXTex/archive/refs/tags/jun2024.zip
)
FetchContent_MakeAvailable(dx_texture_kit)

FetchContent_Declare(
    dx_mesh_kit
    URL https://github.com/microsoft/DirectXMesh/archive/refs/tags/jun2024.zip
)
FetchContent_MakeAvailable(dx_mesh_kit)

FetchContent_Declare(
    dx_math
    URL https://github.com/microsoft/DirectXMath/archive/refs/tags/feb2024.zip
)
FetchContent_MakeAvailable(dx_math)

FetchContent_Declare(
    imgui_external
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

FetchContent_Declare(
    tracy_profiler
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG v0.11.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy_profiler)

option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
add_compile_definitions(TRACY_ENABLE)
# add_subdirectory(${tracy_profiler_SOURCE_DIR}/tracy)  # target: TracyClient or alias Tracy::TracyClient
add_library(tracy
	${tracy_profiler_SOURCE_DIR}/public/TracyClient.cpp
)
target_include_directories(tracy PUBLIC ${tracy_profiler_SOURCE_DIR}/public/tracy/)

add_library(imgui
	${imgui_external_SOURCE_DIR}/imgui.cpp
	${imgui_external_SOURCE_DIR}/imgui_draw.cpp
	${imgui_external_SOURCE_DIR}/imgui_tables.cpp
	${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_external_SOURCE_DIR}/imgui_demo.cpp
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
    ${imgui_external_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR}/backends/)

# file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
# add_library("ImGui" STATIC imgui)
# target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
# target_link_libraries("ImGui" PUBLIC ImGui)

include_directories(
    C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um
    C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared
    ${CMAKE_BINARY_DIR}/_deps/dx_headers-src/include/
    ${CMAKE_BINARY_DIR}/_deps/dx_math-src/Inc/
    ./src/dx_sample_helper_modified/
    ./src/tiny_obj_loader/
    )

link_directories(
    C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/um/x64
)

link_libraries(
    d3d12.lib
    dxgi.lib
    dxguid.lib
    d3dcompiler.lib
)

add_subdirectory(./src)
