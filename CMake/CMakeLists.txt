set(vtkAddon_version "b1fa5034077fc04b10457fe25004c65af6091a37")

set(vtkMacroKitPythonWrap.cmake_expected_hash "745545e34962908dedbd82f6927a57798feb03f993e7f843462d7b44d06156a4")
set(vtkWrapHierarchy.cmake_expected_hash "c9343c54675e8bbde8ceaedf99a3226546b5f7d4cf7d5dac3e33db9093b536f2")
set(vtkWrapPython.cmake_expected_hash "c81e1554f9d41f6333ac8a123ceb75996bda825f7454efbda1b8b2c5fff9b1ea")
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
