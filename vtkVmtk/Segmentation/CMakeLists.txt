#set(VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES vtkvmtkDifferentialGeometry vtkvmtkITK ${ITK_LIBRARIES})
set(VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES vtkvmtkDifferentialGeometry ${ITK_LIBRARIES})

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_SEGMENTATION_COMPONENTS
    vtkCommonComputationalGeometry
    vtkCommonCore
    vtkFiltersCore
    vtkImagingGeneral
    vtkIOImage
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_SEGMENTATION_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_SEGMENTATION_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
  #if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  #  list(APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES
  #    vtkvmtkITKPythonD
  #    )
  #endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkImaging
    )
  #if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  #  list(APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES
  #    vtkvmtkITKPythonD
  #    )
  #endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

SET (VTK_VMTK_SEGMENTATION_SRCS
  vtkvmtkActiveTubeFilter.cxx
  vtkvmtkBoundedReciprocalImageFilter.cxx
  vtkvmtkCardinalSpline.cxx
  vtkvmtkCollidingFrontsImageFilter.cxx
#  vtkvmtkConstrainedLaplacianPolyDataFilter.cxx
  vtkvmtkCurvesLevelSetImageFilter.cxx
  vtkvmtkDanielssonDistanceMapImageFilter.cxx
  vtkvmtkFastMarchingDirectionalFreezeImageFilter.cxx
  vtkvmtkFastMarchingUpwindGradientImageFilter.cxx
  vtkvmtkFWHMFeatureImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSetImageFilter.cxx
  vtkvmtkGeodesicActiveContourLevelSet2DImageFilter.cxx
  vtkvmtkGradientMagnitudeImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter.cxx
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter.cxx
  vtkvmtkGrayscaleMorphologyImageFilter.cxx
  vtkvmtkLaplacianSegmentationLevelSetImageFilter.cxx
  vtkvmtkNormalizeImageFilter.cxx
  vtkvmtkObjectnessMeasureImageFilter.cxx
  vtkvmtkOtsuMultipleThresholdsImageFilter.cxx
  vtkvmtkPolyDataPotentialFit.cxx
  vtkvmtkRecursiveGaussianImageFilter.cxx
  vtkvmtkRecursiveGaussian2DImageFilter.cxx
  vtkvmtkSatoVesselnessMeasureImageFilter.cxx
  vtkvmtkSigmoidImageFilter.cxx
  vtkvmtkThresholdSegmentationLevelSetImageFilter.cxx
  vtkvmtkUpwindGradientMagnitudeImageFilter.cxx
  vtkvmtkVesselEnhancingDiffusionImageFilter.cxx
  #vtkvmtkVesselEnhancingDiffusion3DImageFilter.cxx
  vtkvmtkVesselnessMeasureImageFilter.cxx
  )

SET (VTK_VMTK_SEGMENTATION_ITK_HEADERS
  vtkvmtkITKFilterUtilities.h
  itkFWHMFeatureImageFilter.h
  itkFWHMFeatureImageFilter.txx
  itkFastMarchingDirectionalFreezeImageFilter.h
  itkFastMarchingDirectionalFreezeImageFilter.txx
  itkFastMarchingUpwindGradientImageFilter.h
  itkFastMarchingUpwindGradientImageFilter.txx
  itkUpwindGradientMagnitudeImageFilter.h
  itkUpwindGradientMagnitudeImageFilter.txx
  itkVesselEnhancingDiffusion3DImageFilter.h
  itkVesselEnhancingDiffusion3DImageFilter.txx
)

#SET (VTK_VMTK_SEGMENTATION_INCLUDE_DIRS
#  ${vtkvmtkITK_SOURCE_DIR}
#  ${vtkvmtkITK_BINARY_DIR}
#  )

vmtk_build_library(
  NAME Segmentation
  SRCS ${VTK_VMTK_SEGMENTATION_SRCS} 
  TARGET_LINK_LIBRARIES ${VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES}
  INCLUDE_DIRECTORIES ${VTK_VMTK_SEGMENTATION_INCLUDE_DIRS}
  )
