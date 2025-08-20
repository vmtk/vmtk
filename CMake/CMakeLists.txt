set(vtkAddon_version "cf1126568415179fb5b1c16121087bd2494e2e0e")

set(vtkMacroKitPythonWrap.cmake_expected_hash "82abf4ea2c6f153c61dd47bf6f3baf924d0f54dd11fd92a09cb5dc0f82a85e48")
set(vtkWrapHierarchy.cmake_expected_hash "c9343c54675e8bbde8ceaedf99a3226546b5f7d4cf7d5dac3e33db9093b536f2")
set(vtkWrapPython.cmake_expected_hash "ac22023ff10c58da33c2a6633fc90fde9863f12bb0f90dd0da51e18796137db3")
set(vtkWrapperInit.data.in_expected_hash "3e840679603eb4a0d425c1244828729cb271f33a081c5ac2e0547aa747bbec5d")

foreach(filename IN ITEMS
  vtkMacroKitPythonWrap.cmake
  vtkWrapHierarchy.cmake
  vtkWrapPython.cmake
  vtkWrapperInit.data.in
  )
  set(dest_file "${CMAKE_CURRENT_BINARY_DIR}/${filename}")
  set(expected_hash "${${filename}_expected_hash}")
  set(url "https://raw.githubusercontent.com/Slicer/vtkAddon/${vtkAddon_version}/CMake/${filename}")
  if(NOT EXISTS ${dest_file})
    file(DOWNLOAD ${url} ${dest_file} EXPECTED_HASH SHA256=${expected_hash})
  else()
    file(SHA256 ${dest_file} current_hash)
    if(NOT ${current_hash} STREQUAL ${expected_hash})
      file(DOWNLOAD ${url} ${dest_file} EXPECTED_HASH SHA256=${expected_hash})
    endif()
  endif()
endforeach()

set(vtkAddon_CMAKE_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Path to vtkAddon CMake directory")
