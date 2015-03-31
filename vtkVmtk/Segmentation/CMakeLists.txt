set(VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES vtkvmtkDifferentialGeometry vtkvmtkITK ${ITK_LIBRARIES})

  list( APPEND VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkImaging
    )
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
  vtkvmtkVesselEnhancingDiffusion3DImageFilter.cxx
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

SET (VTK_VMTK_SEGMENTATION_SRCS 
  ${VTK_VMTK_SEGMENTATION_SRCS}
  )

INCLUDE_DIRECTORIES (${vtkvmtkITK_SOURCE_DIR})
INCLUDE_DIRECTORIES (${vtkvmtkITK_BINARY_DIR})


set(lib_name vtkvmtkSegmentation)

ADD_LIBRARY (${lib_name} ${VTK_VMTK_SEGMENTATION_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(${lib_name} ${VTK_VMTK_SEGMENTATION_TARGET_LINK_LIBRARIES})


INSTALL(TARGETS ${lib_name}
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${VTK_VMTK_SEGMENTATION_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Segmentation/*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)
INSTALL(FILES ${VTK_VMTK_SEGMENTATION_ITK_HEADERS}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS ${lib_name})

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###



IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${VTK_VMTK_SEGMENTATION_SRCS}
    KIT_PYTHON_LIBRARIES vtkvmtkDifferentialGeometryPythonD
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
