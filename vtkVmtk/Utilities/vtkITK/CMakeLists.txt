PROJECT (VTKITK)

#SET( VTKITK_SRCS
#vtkITKImageToImageFilter.h
#vtkITKImageToImageFilter2DFF.h
#vtkITKImageToImageFilterFF.h
#vtkITKImageToImageFilterF2F.h
#vtkITKImageToImageFilterF3F3.h
#vtkITKImageToImageFilterFUL.h
#vtkITKImageToImageFilterULUL.h
#vtkITKImageToImageFilterSF.h
#vtkITKImageToImageFilterSUL.h
#vtkITKImageToImageFilterUSUL.h
#vtkITKImageToImageFilterUSUS.h
#vtkITKImageToImageFilterSS.h
#)

#SET_SOURCE_FILES_PROPERTIES ( 
#vtkITKImageToImageFilter.h
#vtkITKImageToImageFilter2DFF.h
#vtkITKImageToImageFilterFF.h
#vtkITKImageToImageFilterF2F.h
#vtkITKImageToImageFilterF3F3.h
#vtkITKImageToImageFilterFUL.h
#vtkITKImageToImageFilterULUL.h
#vtkITKImageToImageFilterSF.h
#vtkITKImageToImageFilterSUL.h
#vtkITKImageToImageFilterUSUL.h
#vtkITKImageToImageFilterUSUS.h
#vtkITKImageToImageFilterSS.h
#ABSTRACT
#)

SET (VTKITK_SRCS
  vtkITKImageToImageFilter.cxx
  vtkITKImageToImageFilter2DFF.cxx
  vtkITKImageToImageFilterFF.cxx
  )

SET_SOURCE_FILES_PROPERTIES ( 
  vtkITKImageToImageFilter.cxx
  vtkITKImageToImageFilter2DFF.cxx
  vtkITKImageToImageFilterFF.cxx
  ABSTRACT
  )

SET (VTKITK_EXTRA_INCLUDES
  vtkITKUtility.h
  )

IF(BORLAND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w-8004")
ENDIF(BORLAND)

INCLUDE_DIRECTORIES(${VTKITK_SOURCE_DIR})
INCLUDE_DIRECTORIES(${VTKITK_BINARY_DIR})

ADD_LIBRARY (vtkVTKITK ${VTKITK_SRCS})
SET_TARGET_PROPERTIES(vtkVTKITK PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(vtkVTKITK vtkImaging ITKIO ITKNumerics ITKBasicFilters)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkVTKITK)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTKITK_SRCS} ${VTKITK_EXTRA_INCLUDES})

# Configure Python wrappers.
IF (VTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkVTKITKPython VTKITKPython_SRCS "${VTKITK_SRCS}")
  ADD_LIBRARY(vtkVTKITKPythonD ${VTKITKPython_SRCS})
  ADD_LIBRARY(vtkVTKITKPython MODULE vtkVTKITKPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkVTKITKPythonD vtkVTKITK vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD)
  TARGET_LINK_LIBRARIES (vtkVTKITKPython vtkVTKITKPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkVTKITKPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkVTKITKPython)
ENDIF (VTK_WRAP_PYTHON)

