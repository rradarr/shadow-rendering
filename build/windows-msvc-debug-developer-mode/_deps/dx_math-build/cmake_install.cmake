# Install script for directory: E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake"
         "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-build/CMakeFiles/Export/be317da9b36e3fc86099e24848d0d665/DirectXMath-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxmath/DirectXMath-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmath" TYPE FILE FILES "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-build/CMakeFiles/Export/be317da9b36e3fc86099e24848d0d665/DirectXMath-targets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/directxmath" TYPE FILE FILES
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXCollision.h"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXCollision.inl"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXColors.h"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXMath.h"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXMathConvert.inl"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXMathMatrix.inl"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXMathMisc.inl"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXMathVector.inl"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXPackedVector.h"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-src/Inc/DirectXPackedVector.inl"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxmath" TYPE FILE FILES
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-build/directxmath-config.cmake"
    "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-build/directxmath-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "E:/programistyczne/Masters Project/build/windows-msvc-debug-developer-mode/_deps/dx_math-build/DirectXMath.pc")
endif()

