project(VTK_VMTK)

if(NOT VMTK_USE_VTK9)
  set(VTK_COMPONENT_PREFIX "vtk")
  set(VTK_TARGET_PREFIX "vtk")
else()
  set(VTK_COMPONENT_PREFIX "")
  set(VTK_TARGET_PREFIX "VTK::")
endif()

include(${VMTK_SOURCE_DIR}/CMake/vmtkFunctions.cmake)

set(dirs
  Utilities
  Common
  ComputationalGeometry
  DifferentialGeometry
  IO
  Misc
  Segmentation
  )

if (VMTK_USE_RENDERING)
  list(APPEND dirs Rendering)
endif ()

option(VTK_VMTK_CONTRIB "Build and install classes in the vtkVmtk/Contrib directory." OFF)

if (VTK_VMTK_CONTRIB)
  list(APPEND dirs Contrib)
endif ()

include (${VMTK_SOURCE_DIR}/CMake/CMakeOptions.cmake)

configure_file(
  ${VTK_VMTK_SOURCE_DIR}/vtkvmtkConfigure.h.in
  ${VTK_VMTK_BINARY_DIR}/vtkvmtkConfigure.h
  )                                                                                                  

if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
endif()

option(VTK_VMTK_BUILD_TETGEN "Build TetGen and TetGen wrapper. Check TetGen license before you activate this." ON)

option(VTK_VMTK_BUILD_STREAMTRACER "Build static temporal stream tracer." ON)

if (VTK_USE_COCOA)
  option(VTK_VMTK_USE_COCOA "Build the Cocoa vmtk classes." ON)
endif ()

if(NOT VTK_VMTK_INSTALL_BIN_DIR)
  set(VTK_VMTK_INSTALL_BIN_DIR bin)
endif()

if(NOT VTK_VMTK_INSTALL_INCLUDE_DIR)
  set(VTK_VMTK_INSTALL_INCLUDE_DIR include/vmtk)
endif()

if(NOT VTK_VMTK_INSTALL_LIB_DIR)
  set(VTK_VMTK_INSTALL_LIB_DIR lib)
endif()

if(NOT VTK_VMTK_MODULE_INSTALL_LIB_DIR)
  set(VTK_VMTK_MODULE_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
endif()

include_directories (${VTK_VMTK_SOURCE_DIR})
include_directories (${VTK_VMTK_SOURCE_DIR}/Common)
include_directories (${VTK_VMTK_SOURCE_DIR}/ComputationalGeometry)
include_directories (${VTK_VMTK_SOURCE_DIR}/DifferentialGeometry)
include_directories (${VTK_VMTK_SOURCE_DIR}/IO)
include_directories (${VTK_VMTK_SOURCE_DIR}/Misc)
if (VMTK_USE_RENDERING)
  include_directories (${VTK_VMTK_SOURCE_DIR}/Rendering)
endif ()
include_directories (${VTK_VMTK_SOURCE_DIR}/Segmentation)
if (VTK_VMTK_CONTRIB)
  include_directories (${VTK_VMTK_SOURCE_DIR}/Contrib)
endif ()
include_directories (${VTK_VMTK_SOURCE_DIR}/Utilities/vtkvmtkITK)
include_directories (${VTK_VMTK_BINARY_DIR})

set(VTK_DOXYGEN_HOME ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen)

file(GLOB files "${VTK_VMTK_BINARY_DIR}/vtkvmtkConfigure*.h")
install(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  include_directories (${VTK_PYTHON_INCLUDE_DIR})
  configure_file(${VTK_VMTK_SOURCE_DIR}/vtkvmtk.py ${VTK_VMTK_BINARY_DIR}/vtkvmtk.py COPYONLY)
  install(FILES ${VTK_VMTK_BINARY_DIR}/vtkvmtk.py
    DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR}
    COMPONENT PythonRuntimeLibraries)
endif ()

set(TETGEN_SOURCE_DIR "${VTK_VMTK_SOURCE_DIR}/Utilities/tetgen1.4.3")

foreach(dir ${dirs})
  add_subdirectory(${dir})
endforeach()
