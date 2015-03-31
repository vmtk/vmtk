# --------------------------------------------------------------------------
# Sources

# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

#FIND_PACKAGE(VTK REQUIRED)
#INCLUDE(${VTK_USE_FILE})

#FIND_PACKAGE(ITK REQUIRED)
#INCLUDE(${ITK_USE_FILE})

SET(vtkvmtkITK_SRCS
    vtkvmtkITKArchetypeImageSeriesReader.cxx
    vtkvmtkITKArchetypeImageSeriesScalarReader.cxx
    vtkvmtkITKImageWriter.cxx
    vtkvmtkITKImageToImageFilter.h
    vtkvmtkITKImageToImageFilter2DFF.h
    vtkvmtkITKImageToImageFilterFF.h
)

SET(VTK_VMTK_ITK_EXTRA_INCLUDES
  vtkvmtkITK.h
  vtkvmtkITKUtility.h
)

# Abstract/pure virtual classes

#SET_SOURCE_FILES_PROPERTIES (
#  ABSTRACT
#)

# Helper classes

#SET_SOURCE_FILES_PROPERTIES(
#  WRAP_EXCLUDE
#  )

# --------------------------------------------------------------------------
# Include dirs

#INCLUDE_DIRECTORIES(
#  ${vtkvmtkITK_SOURCE_DIR}
#  ${vtkvmtkITK_BINARY_DIR}
#  ${ITK_INCLUDE_DIRS}
#  ${VTK_INCLUDE_DIRS}
#  )

# --------------------------------------------------------------------------
# Build the library

ADD_LIBRARY (vtkvmtkITK ${vtkvmtkITK_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES LINKER_LANGUAGE CXX)
IF(NOT WIN32)
  SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(NOT WIN32)
TARGET_LINK_LIBRARIES(vtkvmtkITK vtkImaging ${ITK_LIBRARIES})


INSTALL(TARGETS vtkvmtkITK
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${vtkvmtkITK_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Utilities/vtkvmtkITK/*.h")
INSTALL(FILES ${files} ${VTK_VMTK_ITK_EXTRA_INCLUDES}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS vtkvmtkITK)

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###



# --------------------------------------------------------------------------
# Wrapping

#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
#VTK_WRAP_TCL3(vtkvmtkITK vtkvmtkITK_TCL_SRCS "${vtkvmtkITK_SRCS}" "")
IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkITKPython vtkvmtkITKPython_SRCS "${vtkvmtkITK_SRCS}")
  ADD_LIBRARY(vtkvmtkITKPythonD ${vtkvmtkITKPython_SRCS})
  IF(VMTK_LIBRARY_PROPERTIES)
    SET_TARGET_PROPERTIES(vtkvmtkITKPythonD PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
  ENDIF(VMTK_LIBRARY_PROPERTIES)
  ADD_LIBRARY(vtkvmtkITKPython MODULE vtkvmtkITKPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkITKPythonD vtkvmtkITK vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkITKPython vtkvmtkITKPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkITKPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkITKPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
    )
  INSTALL(TARGETS vtkvmtkITKPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
