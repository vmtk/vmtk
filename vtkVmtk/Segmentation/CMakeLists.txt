SET (VTK_VMTK_SEGMENTATION_SRCS
  vtkvmtkBoundedReciprocalImageFilter.cxx
  vtkvmtkDanielssonDistanceMapImageFilter.cxx
  vtkvmtkCollidingFrontsImageFilter.cxx
  vtkvmtkCurvesLevelSetImageFilter.cxx
  vtkvmtkFastMarchingUpwindGradientImageFilter.cxx
  vtkvmtkFWHMFeatureImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSetImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSet2DImageFilter.cxx
  vtkvmtkGradientMagnitudeImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter.cxx
#  vtkvmtkObjectnessMeasureImageFilter.cxx
#  vtkvmtkObjectnessMeasure2DImageFilter.cxx
  vtkvmtkRecursiveGaussianImageFilter.cxx
  vtkvmtkRecursiveGaussian2DImageFilter.cxx
  vtkvmtkSigmoidImageFilter.cxx
  vtkvmtkUpwindGradientMagnitudeImageFilter.cxx
  vtkvmtkPolyDataPotentialFit.cxx
#  vtkvmtkConstrainedLaplacianPolyDataFilter.cxx
#  vtkvmtkVesselnessMeasureImageFilter.cxx
  )

ADD_LIBRARY (vtkvmtkSegmentation ${VTK_VMTK_SEGMENTATION_SRCS})
SET_TARGET_PROPERTIES(vtkvmtkSegmentation PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(vtkvmtkSegmentation vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkImaging vtkITK ITKCommon ITKBasicFilters)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkSegmentation)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_SEGMENTATION_SRCS})

INCLUDE_DIRECTORIES (${vtkITK_SOURCE_DIR})
INCLUDE_DIRECTORIES (${vtkITK_BINARY_DIR})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkSegmentationPython VTK_VMTK_SEGMENTATION_PYTHON_SRCS "${VTK_VMTK_SEGMENTATION_SRCS}")
  ADD_LIBRARY(vtkvmtkSegmentationPythonD ${VTK_VMTK_SEGMENTATION_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkSegmentationPython MODULE vtkvmtkSegmentationPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkSegmentationPythonD vtkvmtkSegmentation vtkvmtkDifferentialGeometryPythonD vtkvmtkDifferentialGeometry vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD vtkImaging vtkImagingPythonD vtkITKPythonD vtkITK)
  TARGET_LINK_LIBRARIES (vtkvmtkSegmentationPython vtkvmtkSegmentationPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkSegmentationPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkSegmentationPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
