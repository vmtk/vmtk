SET (VTK_VMTK_SEGMENTATION_SRCS
  vtkvmtkBoundedReciprocalImageFilter.cxx
  vtkvmtkDanielssonDistanceMapImageFilter.cxx
  vtkvmtkCollidingFrontsImageFilter.cxx
  vtkvmtkCurvesLevelSetImageFilter.cxx
  vtkvmtkFastMarchingDirectionalFreezeImageFilter.cxx
  vtkvmtkFastMarchingUpwindGradientImageFilter.cxx
  vtkvmtkFWHMFeatureImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSetImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSet2DImageFilter.cxx
  vtkvmtkGradientMagnitudeImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter.cxx
#  vtkvmtkObjectnessMeasureImageFilter.cxx
#  vtkvmtkObjectnessMeasure2DImageFilter.cxx
  vtkvmtkLaplacianSegmentationLevelSetImageFilter.cxx
  vtkvmtkRecursiveGaussianImageFilter.cxx
  vtkvmtkRecursiveGaussian2DImageFilter.cxx
  vtkvmtkSigmoidImageFilter.cxx
  vtkvmtkThresholdSegmentationLevelSetImageFilter.cxx
  vtkvmtkUpwindGradientMagnitudeImageFilter.cxx
  vtkvmtkPolyDataPotentialFit.cxx
#  vtkvmtkConstrainedLaplacianPolyDataFilter.cxx
  vtkvmtkVesselnessMeasureImageFilter.cxx
  )

IF (NOT VMTK_EXTERNAL_vtkITK)
SET (VTK_VMTK_SEGMENTATION_SRCS 
  ${VTK_VMTK_SEGMENTATION_SRCS}
#  vtkvmtkObjectnessMeasureImageFilter.cxx
#  vtkvmtkObjectnessMeasure2DImageFilter.cxx
  )
ENDIF (NOT VMTK_EXTERNAL_vtkITK)

IF (NOT VMTK_EXTERNAL_vtkITK)
  INCLUDE_DIRECTORIES (${vtkITK_SOURCE_DIR})
  INCLUDE_DIRECTORIES (${vtkITK_BINARY_DIR})
ELSE (NOT VMTK_EXTERNAL_vtkITK)
  INCLUDE_DIRECTORIES (${VMTK_vtkITK_SOURCE_DIR})
  INCLUDE_DIRECTORIES (${VMTK_vtkITK_BINARY_DIR})
  LINK_DIRECTORIES (${VMTK_vtkITK_LIBRARY_DIR})
ENDIF (NOT VMTK_EXTERNAL_vtkITK)

ADD_LIBRARY (vtkvmtkSegmentation ${VTK_VMTK_SEGMENTATION_SRCS})
SET_TARGET_PROPERTIES(vtkvmtkSegmentation PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(vtkvmtkSegmentation vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkImaging vtkvmtkITK ITKCommon ITKBasicFilters)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkSegmentation)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_SEGMENTATION_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkSegmentationPython VTK_VMTK_SEGMENTATION_PYTHON_SRCS "${VTK_VMTK_SEGMENTATION_SRCS}")
  ADD_LIBRARY(vtkvmtkSegmentationPythonD ${VTK_VMTK_SEGMENTATION_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkSegmentationPython MODULE vtkvmtkSegmentationPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkSegmentationPythonD vtkvmtkSegmentation vtkvmtkDifferentialGeometryPythonD vtkvmtkDifferentialGeometry vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD vtkImaging vtkImagingPythonD vtkvmtkITKPythonD vtkvmtkITK)
  TARGET_LINK_LIBRARIES (vtkvmtkSegmentationPython vtkvmtkSegmentationPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkSegmentationPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkSegmentationPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkSegmentationPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkSegmentationTCL VTK_VMTK_SEGMENTATION_TCL_SRCS "${VTK_VMTK_SEGMENTATION_SRCS}" "")
  ADD_LIBRARY(vtkvmtkSegmentationTCL ${VTK_VMTK_SEGMENTATION_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkSegmentationTCL vtkvmtkSegmentation vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL vtkImaging vtkImagingTCL vtkvmtkITK vtkvmtkITKTCL)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkSegmentationTCL)
ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
