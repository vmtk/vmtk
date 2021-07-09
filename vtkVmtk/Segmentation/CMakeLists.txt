set(VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES vtkvmtkDifferentialGeometry ${ITK_LIBRARIES})

set( VTK_VMTK_SEGMENTATION_COMPONENTS
  ${VTK_COMPONENT_PREFIX}CommonComputationalGeometry
  ${VTK_COMPONENT_PREFIX}CommonCore
  ${VTK_COMPONENT_PREFIX}FiltersCore
  ${VTK_COMPONENT_PREFIX}ImagingGeneral
  ${VTK_COMPONENT_PREFIX}IOImage
  )
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND VTK_VMTK_SEGMENTATION_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_SEGMENTATION_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
#if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
#  list(APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES
#    vtkvmtkITKPythonD
#    )
#endif()

set (VTK_VMTK_SEGMENTATION_SRCS
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
  vtkvmtkMedialCurveFilter.cxx
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
  vtkvmtkAnisotropicDiffusionImageFilter.cxx
  )

set (VTK_VMTK_SEGMENTATION_ITK_HEADERS
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
  itkAverageOutwardFluxImageFilter.h
  itkAverageOutwardFluxImageFilter.txx
  itkMedialCurveImageFilter.h
  itkMedialCurveImageFilter.txx
)

vmtk_build_library(
  NAME Segmentation
  SRCS ${VTK_VMTK_SEGMENTATION_SRCS} 
  TARGET_LINK_LIBRARIES ${VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES}
  )
