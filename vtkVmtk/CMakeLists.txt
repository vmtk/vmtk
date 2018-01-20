PROJECT(VTK_VMTK)

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

IF (VMTK_USE_RENDERING)
  list(APPEND dirs Rendering)
ENDIF (VMTK_USE_RENDERING)

OPTION(VTK_VMTK_CONTRIB "Build and install classes in the vtkVmtk/Contrib directory." OFF)

IF (VTK_VMTK_CONTRIB)
  list(APPEND dirs Contrib)
ENDIF (VTK_VMTK_CONTRIB)

INCLUDE (${VMTK_SOURCE_DIR}/CMake/CMakeOptions.cmake)

CONFIGURE_FILE(
  ${VTK_VMTK_SOURCE_DIR}/vtkvmtkConfigure.h.in
  ${VTK_VMTK_BINARY_DIR}/vtkvmtkConfigure.h
  )                                                                                                  

if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
endif(APPLE)

OPTION(VTK_VMTK_BUILD_TETGEN "Build TetGen and TetGen wrapper. Check TetGen license before you activate this." ON)

OPTION(VTK_VMTK_BUILD_STREAMTRACER "Build static temporal stream tracer." ON)

IF (VTK_USE_COCOA)
  OPTION(VTK_VMTK_USE_COCOA "Build the Cocoa vmtk classes." ON)
ENDIF (VTK_USE_COCOA)

IF(NOT VTK_VMTK_INSTALL_BIN_DIR)
  SET(VTK_VMTK_INSTALL_BIN_DIR bin)
ENDIF(NOT VTK_VMTK_INSTALL_BIN_DIR)

IF(NOT VTK_VMTK_INSTALL_INCLUDE_DIR)
  SET(VTK_VMTK_INSTALL_INCLUDE_DIR include/vmtk)
ENDIF(NOT VTK_VMTK_INSTALL_INCLUDE_DIR)

IF(NOT VTK_VMTK_INSTALL_LIB_DIR)
  SET(VTK_VMTK_INSTALL_LIB_DIR lib)
ENDIF(NOT VTK_VMTK_INSTALL_LIB_DIR)

IF(NOT VTK_VMTK_MODULE_INSTALL_LIB_DIR)
  SET(VTK_VMTK_MODULE_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
ENDIF(NOT VTK_VMTK_MODULE_INSTALL_LIB_DIR)

INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR})
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Common)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/ComputationalGeometry)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/DifferentialGeometry)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/IO)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Misc)
IF (VMTK_USE_RENDERING)
  INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Rendering)
ENDIF (VMTK_USE_RENDERING)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Segmentation)
IF (VTK_VMTK_CONTRIB)
  INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Contrib)
ENDIF (VTK_VMTK_CONTRIB)
INCLUDE_DIRECTORIES (${VTK_VMTK_SOURCE_DIR}/Utilities/vtkvmtkITK)
INCLUDE_DIRECTORIES (${VTK_VMTK_BINARY_DIR})

SET(VTK_DOXYGEN_HOME ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen)

FILE(GLOB files "${VTK_VMTK_BINARY_DIR}/vtkvmtkConfigure*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  INCLUDE_DIRECTORIES (${VTK_PYTHON_INCLUDE_DIR})
  CONFIGURE_FILE(${VTK_VMTK_SOURCE_DIR}/vtkvmtk.py ${VTK_VMTK_BINARY_DIR}/vtkvmtk.py COPYONLY)
  INSTALL(FILES ${VTK_VMTK_BINARY_DIR}/vtkvmtk.py
    DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR}
    COMPONENT PythonRuntimeLibraries)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

SET(TETGEN_SOURCE_DIR "${VTK_VMTK_SOURCE_DIR}/Utilities/tetgen1.4.3")

foreach(dir ${dirs})
  add_subdirectory(${dir})
endforeach()
