# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-src")
  file(MAKE_DIRECTORY "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-src")
endif()
file(MAKE_DIRECTORY
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-build"
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix"
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/tmp"
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/src/dx_headers-populate-stamp"
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/src"
  "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/src/dx_headers-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/src/dx_headers-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/programistyczne/Masters Project/test2/build/windows-x64/_deps/dx_headers-subbuild/dx_headers-populate-prefix/src/dx_headers-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
