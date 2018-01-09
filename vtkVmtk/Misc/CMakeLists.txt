set(VTK_VMTK_MISC_TARGET_LINK_LIBRARIES vtkvmtkComputationalGeometry vtkvmtkDifferentialGeometry)

set( VTK_VMTK_MISC_COMPONENTS
vtkFiltersFlowPaths
vtkFiltersGeometry
vtkFiltersModeling
vtkImagingCore
)
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
list(APPEND VTK_VMTK_MISC_COMPONENTS
  vtkWrappingPythonCore
  )
endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_MISC_COMPONENTS} REQUIRED )
include( ${VTK_USE_FILE} )
list( APPEND VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

SET (VTK_VMTK_MISC_SRCS
  vtkvmtkAnnularCapPolyData.cxx
  vtkvmtkBoundaryLayerGenerator.cxx
  vtkvmtkCurvedMPRImageFilter.cxx
  vtkvmtkImageBoxPainter.cxx
  vtkvmtkIterativeClosestPointTransform.cxx
  vtkvmtkLevelSetSigmoidFilter.cxx
  vtkvmtkLinearizeMeshFilter.cxx
  vtkvmtkLinearToQuadraticMeshFilter.cxx
  vtkvmtkLinearToQuadraticSurfaceMeshFilter.cxx
  vtkvmtkMeshLambda2.cxx
  vtkvmtkMeshProjection.cxx
  vtkvmtkMeshVelocityStatistics.cxx
  vtkvmtkMeshVorticity.cxx
  vtkvmtkMeshWallShearRate.cxx
  vtkvmtkPolyDataNetworkExtraction.cxx
  vtkvmtkPolyDataNormalPlaneEstimator.cxx
  vtkvmtkPolyDataKiteRemovalFilter.cxx
  vtkvmtkPolyDataSizingFunction.cxx
  vtkvmtkPolyDataToUnstructuredGridFilter.cxx
  vtkvmtkRBFInterpolation.cxx
  vtkvmtkSimpleCapPolyData.cxx
  vtkvmtkSmoothCapPolyData.cxx
  vtkvmtkStaticTemporalInterpolatedVelocityField.cxx
  vtkvmtkStaticTemporalStreamTracer.cxx
  vtkvmtkStreamlineClusteringFilter.cxx
  vtkvmtkStreamlineOsculatingCentersFilter.cxx
  vtkvmtkStreamlineToParticlesFilter.cxx
  vtkvmtkSurfaceDistance.cxx
  vtkvmtkSurfaceProjection.cxx
  vtkvmtkTopologicalSeamFilter.cxx
  vtkvmtkUnstructuredGridTetraFilter.cxx
  )

IF (DEFINED VMTK_BUILD_TETGEN)
  IF (VMTK_BUILD_TETGEN AND VTK_VMTK_BUILD_TETGEN)
    SET (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkTetGenWrapper.cxx)

    ADD_DEFINITIONS (-DTETLIBRARY)
    INCLUDE_DIRECTORIES(${TETGEN_SOURCE_DIR})
    LINK_DIRECTORIES(${TETGEN_LIB_DIR})

    SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} tet)
  ENDIF (VMTK_BUILD_TETGEN AND VTK_VMTK_BUILD_TETGEN)
ELSE (DEFINED VMTK_BUILD_TETGEN) 
  IF (VTK_VMTK_BUILD_TETGEN)
    SET (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkTetGenWrapper.cxx)

    ADD_DEFINITIONS (-DTETLIBRARY)
    INCLUDE_DIRECTORIES(${TETGEN_SOURCE_DIR})
    LINK_DIRECTORIES(${TETGEN_LIB_DIR})

    SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} tet)
  ENDIF (VTK_VMTK_BUILD_TETGEN)
ENDIF (DEFINED VMTK_BUILD_TETGEN)

IF (VTK_VMTK_BUILD_STREAMTRACER)
  SET (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkStaticTemporalInterpolatedVelocityField.cxx vtkvmtkStaticTemporalStreamTracer.cxx)

ENDIF (VTK_VMTK_BUILD_STREAMTRACER)

vmtk_build_library(
  NAME Misc
  SRCS ${VTK_VMTK_MISC_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES}
  )
