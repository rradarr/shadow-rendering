# Install script for directory: E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/Debug/DirectXTK12.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/Release/DirectXTK12.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/MinSizeRel/DirectXTK12.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/lib/RelWithDebInfo/DirectXTK12.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk12/DirectXTK12-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk12/DirectXTK12-targets.cmake"
         "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk12/DirectXTK12-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk12/DirectXTK12-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/CMakeFiles/Export/dc79b1416ca4e830525952dfb83a96c7/DirectXTK12-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/directxtk12" TYPE FILE FILES
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/BufferHelpers.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/CommonStates.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/DDSTextureLoader.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/DescriptorHeap.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/DirectXHelpers.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/Effects.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/EffectPipelineStateDescription.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/GeometricPrimitive.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/GraphicsMemory.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/Model.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/PostProcess.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/PrimitiveBatch.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/RenderTargetState.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/ResourceUploadBatch.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/ScreenGrab.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/SpriteBatch.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/SpriteFont.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/VertexTypes.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/WICTextureLoader.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/GamePad.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/Keyboard.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/Mouse.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/SimpleMath.h"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/SimpleMath.inl"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-src/Inc/Audio.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk12" TYPE FILE FILES
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/directxtk12-config.cmake"
    "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/directxtk12-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_tool_kit-build/DirectXTK12.pc")
endif()

