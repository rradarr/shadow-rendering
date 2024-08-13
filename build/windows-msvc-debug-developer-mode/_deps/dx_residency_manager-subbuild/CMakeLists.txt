# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.30.1)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(dx_residency_manager-populate NONE)



include(ExternalProject)
ExternalProject_Add(dx_residency_manager-populate
                     "UPDATE_DISCONNECTED" "False" "EXTERNALPROJECT_INTERNAL_ARGUMENT_SEPARATOR" "URL" "https://raw.githubusercontent.com/microsoft/DirectX-Graphics-Samples/master/Libraries/D3DX12Residency/d3dx12Residency.h" DOWNLOAD_NO_EXTRACT YES
                    SOURCE_DIR          "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_residency_manager-src"
                    BINARY_DIR          "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_residency_manager-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


ExternalProject_Get_Property(dx_residency_manager-populate DOWNLOADED_FILE)
get_filename_component(dlFileName "${DOWNLOADED_FILE}" NAME)

ExternalProject_Add_Step(dx_residency_manager-populate copyfile
  COMMAND    "C:/Program Files/CMake/bin/cmake.exe" -E copy_if_different
             "<DOWNLOADED_FILE>" "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_residency_manager-src"
  DEPENDEES  patch
  DEPENDERS  configure
  BYPRODUCTS "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_residency_manager-src/${dlFileName}"
  COMMENT    "Copying file to SOURCE_DIR"
)

