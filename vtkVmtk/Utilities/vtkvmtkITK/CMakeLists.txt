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
    vtkvmtkITKArchetypeImageSeriesVectorReader.cxx
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
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES COMPILE_FLAGS -fPIC)
TARGET_LINK_LIBRARIES(vtkvmtkITK vtkImaging ITKNumerics ITKBasicFilters ITKIO ITKDICOMParser)


INSTALL(TARGETS vtkvmtkITK
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
COMPONENT RuntimeLibraries
)

#FILE(GLOB files "${vtkvmtkITK_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Utilities/vtkvmtkITK/*.h")
INSTALL(FILES ${files} ${VTK_VMTK_ITK_EXTRA_INCLUDES} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

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
  ADD_LIBRARY(vtkvmtkITKPython MODULE vtkvmtkITKPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkITKPythonD vtkvmtkITK vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkITKPython vtkvmtkITKPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkITKPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkITKPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )
  INSTALL(TARGETS vtkvmtkITKPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
#  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkITKTCL vtkvmtkITKTCL_SRCS "${vtkvmtkITK_SRCS}" "")
  ADD_LIBRARY(vtkvmtkITKTCL ${vtkvmtkITKTCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkITKTCL vtkvmtkITK vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkImaging vtkImagingTCL)

  INSTALL(TARGETS vtkvmtkITKTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )

  ### START VMTK IN SLICER PATCH ###
  IF (VTK_VMTK_IN_SLICER)
    SET(LIBS vtkvmtkITKTCL)

    FOREACH(LIB ${LIBS})
      slicer3_set_modules_output_path(${LIB})
    ENDFOREACH(LIB ${LIBS})
    
    slicer3_install_modules(${LIBS})
  ENDIF (VTK_VMTK_IN_SLICER)
  ### END VMTK IN SLICER PATCH ###

ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
# --------------------------------------------------------------------------
# Build the library

#ADD_LIBRARY(vtkvmtkITK 
#  ${vtkvmtkITK_SRCS} 
#  )
#
#IF(Slicer3_SOURCE_DIR)
#  IF(BUILD_SHARED_LIBS)
#  INSTALL_TARGETS(${SLICER3_LIB_INSTALL_DIR} vtkvmtkITK)
#  ENDIF(BUILD_SHARED_LIBS)
#ENDIF(Slicer3_SOURCE_DIR)
#
#TARGET_LINK_LIBRARIES(vtkvmtkITK 
#  vtkCommonTCL
#  vtkFilteringTCL
#  vtkImagingTCL
#  ITKAlgorithms
#  ITKNumerics 
#  ITKCommon
#  ITKBasicFilters
#  ITKNumerics
#  ITKStatistics
#  ITKBasicFilters
#  ITKIO
#  ITKDICOMParser
#  )
#
#
## --------------------------------------------------------------------------
## Testing (requires some of the examples)
#
#IF(BUILD_TESTING)
#  SUBDIRS(Testing)
#ENDIF(BUILD_TESTING)
#
#CONFIGURE_FILE(
#  ${vtkvmtkITK_SOURCE_DIR}/vtkvmtkITKConfigure.h.in 
#  ${vtkvmtkITK_BINARY_DIR}/vtkvmtkITKConfigure.h
#)

#CONFIGURE_FILE(
#  ${vtkvmtkITK_SOURCE_DIR}/vtkvmtkITKConfig.cmake.in 
#  ${vtkvmtkITK_BINARY_DIR}/vtkvmtkITKConfig.cmake
#)
