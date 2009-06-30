# --------------------------------------------------------------------------
# Sources

# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

#FIND_PACKAGE(VTK REQUIRED)
#INCLUDE(${VTK_USE_FILE})

#FIND_PACKAGE(ITK REQUIRED)
#INCLUDE(${ITK_USE_FILE})

SET(vtkITK_SRCS
    vtkITKArchetypeImageSeriesReader.cxx
    vtkITKArchetypeImageSeriesScalarReader.cxx
    vtkITKArchetypeImageSeriesVectorReader.cxx
    vtkITKImageWriter.cxx
    vtkITKImageToImageFilter.h
    vtkITKImageToImageFilter2DFF.h
    vtkITKImageToImageFilterFF.h
)

SET(VTKITK_EXTRA_INCLUDES
  vtkITK.h
  vtkITKUtility.h
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
#  ${vtkITK_SOURCE_DIR}
#  ${vtkITK_BINARY_DIR}
#  ${ITK_INCLUDE_DIRS}
#  ${VTK_INCLUDE_DIRS}
#  )

# --------------------------------------------------------------------------
# Build the library

ADD_LIBRARY (vtkvmtkITK ${vtkITK_SRCS})
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES COMPILE_FLAGS -fPIC)
TARGET_LINK_LIBRARIES(vtkvmtkITK vtkImaging ITKNumerics ITKBasicFilters ITKIO ITKDICOMParser)


INSTALL(TARGETS vtkvmtkITK
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
)

FILE(GLOB files "${vtkITK_SRCS}/*.h")
INSTALL(FILES ${files} ${VTKITK_EXTRA_INCLUDES} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

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
#VTK_WRAP_TCL3(vtkITK vtkITK_TCL_SRCS "${vtkITK_SRCS}" "")
IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkITKPython vtkITKPython_SRCS "${vtkITK_SRCS}")
  ADD_LIBRARY(vtkvmtkITKPythonD ${vtkITKPython_SRCS})
  ADD_LIBRARY(vtkvmtkITKPython MODULE vtkvmtkITKPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkITKPythonD vtkvmtkITK vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkITKPython vtkvmtkITKPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkITKPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkITKPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
  INSTALL(TARGETS vtkvmtkITKPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
#  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkITKTCL vtkITKTCL_SRCS "${vtkITK_SRCS}" "")
  ADD_LIBRARY(vtkvmtkITKTCL ${vtkITKTCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkITKTCL vtkvmtkITK vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkImaging vtkImagingTCL)

  INSTALL(TARGETS vtkvmtkITKTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
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

#ADD_LIBRARY(vtkITK 
#  ${vtkITK_SRCS} 
#  )
#
#IF(Slicer3_SOURCE_DIR)
#  IF(BUILD_SHARED_LIBS)
#  INSTALL_TARGETS(${SLICER3_LIB_INSTALL_DIR} vtkITK)
#  ENDIF(BUILD_SHARED_LIBS)
#ENDIF(Slicer3_SOURCE_DIR)
#
#TARGET_LINK_LIBRARIES(vtkITK 
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
#  ${vtkITK_SOURCE_DIR}/vtkITKConfigure.h.in 
#  ${vtkITK_BINARY_DIR}/vtkITKConfigure.h
#)

#CONFIGURE_FILE(
#  ${vtkITK_SOURCE_DIR}/vtkITKConfig.cmake.in 
#  ${vtkITK_BINARY_DIR}/vtkITKConfig.cmake
#)
