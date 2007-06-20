PROJECT(vtkITK)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# --------------------------------------------------------------------------
# Sources

# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

SET(vtkITK_SRCS
    vtkITKArchetypeImageSeriesReader.cxx
    vtkITKArchetypeImageSeriesScalarReader.cxx
    vtkITKArchetypeImageSeriesVectorReader.cxx
    vtkITKImageWriter.cxx
    vtkITKImageToImageFilter.cxx
    vtkITKImageToImageFilter2DFF.cxx
    vtkITKImageToImageFilterFF.cxx
)

# Abstract/pure virtual classes

#SET_SOURCE_FILES_PROPERTIES(
#  ABSTRACT
#  )

# Helper classes

#SET_SOURCE_FILES_PROPERTIES(
#  WRAP_EXCLUDE
#  )

# --------------------------------------------------------------------------
# Include dirs

INCLUDE_DIRECTORIES(
  ${vtkITK_SOURCE_DIR}
  ${vtkITK_BINARY_DIR}
  ${ITK_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  )

# --------------------------------------------------------------------------
# Build the library

ADD_LIBRARY (vtkvmtkITK ${vtkITK_SRCS})
SET_TARGET_PROPERTIES(vtkvmtkITK PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(vtkvmtkITK vtkImaging ITKNumerics ITKBasicFilters ITKIO ITKDICOMParser)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkITK)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${vtkvmtkITK_SRCS} ${VTKITK_EXTRA_INCLUDES})


# --------------------------------------------------------------------------
# Wrapping

#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
#VTK_WRAP_TCL3(vtkITK vtkITK_TCL_SRCS "${vtkITK_SRCS}" "")
IF (VTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkITKPython vtkITKPython_SRCS "${vtkITK_SRCS}")
  ADD_LIBRARY(vtkvmtkITKPythonD ${vtkITKPython_SRCS})
  ADD_LIBRARY(vtkvmtkITKPython MODULE vtkvmtkITKPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkITKPythonD vtkvmtkITK vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkITKPython vtkvmtkITKPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkITKPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkITKPython)
ENDIF (VTK_WRAP_PYTHON)

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
CONFIGURE_FILE(
  ${vtkITK_SOURCE_DIR}/vtkITKConfigure.h.in 
  ${vtkITK_BINARY_DIR}/vtkITKConfigure.h
)

CONFIGURE_FILE(
  ${vtkITK_SOURCE_DIR}/vtkITKConfig.cmake.in 
  ${vtkITK_BINARY_DIR}/vtkITKConfig.cmake
)
