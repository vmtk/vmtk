# Download vtkAddon CMake modules to support VTK python wrapping
set(vtkAddon_version "8b5c4b2336a4d2d2aaafe87db3642b5302ddcaa5")

set(vtkMacroKitPythonWrap.cmake_expected_hash "4470613601f97d36221a7af072731475d06f03d4eb19f550d230bc468264088b")
set(vtkWrapHierarchy.cmake_expected_hash "ca3ea938ae7eb7c94da8a99d1c45556de5c0562ab91f7b2a5c5640e50f005a88")
set(vtkWrapPython.cmake_expected_hash "4bdd1adcd02b7db393c32ae902f208389fb80141584e5b68f7668dd379f62134")
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
