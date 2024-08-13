cmake_minimum_required(VERSION 3.3.0)
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

include_directories(
    C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um
    C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared
    ./build/windows-x64/_deps/dx_headers-src/include/
    ./build/windows-x64/_deps/dx_math-src/Inc/
    ./src/dx_sample_helper_modified/
    ./src/tiny_obj_loader/)

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

#target_link_libraries(CmakeTestProject PRIVATE lib_name::module)
#target_link_libraries(CmakeTestProject D3d12.dll)
#target_link_libraries(CmakeTestProject D3d12SDKLayers.dll)