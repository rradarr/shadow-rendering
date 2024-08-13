# Install script for directory: E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/CmakeTestProject")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/Debug/DirectXMesh.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/Release/DirectXMesh.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/MinSizeRel/DirectXMesh.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/RelWithDebInfo/DirectXMesh.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/DirectXMesh.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/DirectXMesh-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/DirectXMesh-targets.cmake"
         "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/DirectXMesh-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/DirectXMesh-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/DirectXMesh-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/Utilities-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/Utilities-targets.cmake"
         "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/Utilities-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/Utilities-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmesh/Utilities-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/CMakeFiles/Export/9a2112bb9e6126dbe84ae170024a6dfc/Utilities-targets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-src/DirectXMesh/DirectXMesh.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-src/DirectXMesh/DirectXMesh.inl"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/utils" TYPE FILE FILES
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-src/Utilities/FlexibleVertexFormat.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-src/Utilities/WaveFrontReader.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmesh" TYPE FILE FILES
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/directxmesh-config.cmake"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_mesh_kit-build/directxmesh-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/bin/Debug/meshconvert.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/bin/Release/meshconvert.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/bin/MinSizeRel/meshconvert.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/bin/RelWithDebInfo/meshconvert.exe")
  endif()
endif()

