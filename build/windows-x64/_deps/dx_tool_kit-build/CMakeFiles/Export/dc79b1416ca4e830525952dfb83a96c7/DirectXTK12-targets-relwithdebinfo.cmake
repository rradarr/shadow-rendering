#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Microsoft::DirectXTK12" for configuration "RelWithDebInfo"
set_property(TARGET Microsoft::DirectXTK12 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Microsoft::DirectXTK12 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/DirectXTK12.lib"
  )

list(APPEND _cmake_import_check_targets Microsoft::DirectXTK12 )
list(APPEND _cmake_import_check_files_for_Microsoft::DirectXTK12 "${_IMPORT_PREFIX}/lib/DirectXTK12.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
