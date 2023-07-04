set(VTK_VMTK_MISC_TARGET_LINK_LIBRARIES vtkvmtkComputationalGeometry vtkvmtkDifferentialGeometry)

# Linking of try_compile fails due to some obscure reason.
# Since for us here is enough to test compilation, we disable linking
# instead of spending time with trying to resolve the linking issue.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
try_compile(_VTK_HAS_VTKINTERVALINFORMATION
  "${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp"
  SOURCES "${CMAKE_CURRENT_LIST_DIR}/vtkvmtkTestIntervalInformation.cxx"
  LINK_LIBRARIES ${VTK_LIBRARIES}
  OUTPUT_VARIABLE _VTK_HAS_VTKINTERVALINFORMATION_OUTPUT)
mark_as_advanced(_VTK_HAS_VTKINTERVALINFORMATION)

if (NOT _VTK_HAS_VTKINTERVALINFORMATION)
  add_definitions (-DVMTK_USE_LEGACY_INTERVAL_INFORMATION)
endif()

set( VTK_VMTK_MISC_COMPONENTS
  ${VTK_COMPONENT_PREFIX}FiltersFlowPaths
  ${VTK_COMPONENT_PREFIX}FiltersGeometry
  ${VTK_COMPONENT_PREFIX}FiltersModeling
  ${VTK_COMPONENT_PREFIX}ImagingCore
  )
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND VTK_VMTK_MISC_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_MISC_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

set (VTK_VMTK_MISC_SRCS
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
  vtkvmtkStreamlineClusteringFilter.cxx
  vtkvmtkStreamlineOsculatingCentersFilter.cxx
  vtkvmtkStreamlineToParticlesFilter.cxx
  vtkvmtkSurfaceDistance.cxx
  vtkvmtkSurfaceProjection.cxx
  vtkvmtkTopologicalSeamFilter.cxx
  vtkvmtkUnstructuredGridTetraFilter.cxx
  )

if (DEFINED VMTK_BUILD_TETGEN)
  if (VMTK_BUILD_TETGEN AND VTK_VMTK_BUILD_TETGEN)
    set (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkTetGenWrapper.cxx)

    add_definitions (-DTETLIBRARY)
    include_directories(${TETGEN_SOURCE_DIR})
    link_directories(${TETGEN_LIB_DIR})

    set (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} tet)
  endif ()
else ()
  if (VTK_VMTK_BUILD_TETGEN)
    set (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkTetGenWrapper.cxx)

    add_definitions (-DTETLIBRARY)
    include_directories(${TETGEN_SOURCE_DIR})
    link_directories(${TETGEN_LIB_DIR})

    set (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} tet)
  endif ()
endif ()

if (VTK_VMTK_BUILD_STREAMTRACER)
  if(${VTK_VERSION} VERSION_GREATER_EQUAL "9.2")
    # Interpolated velocity fields were completely reworked in VTK-9.2
    # (see https://gitlab.kitware.com/vtk/vtk/-/commit/4659e29a49034760520540f004d0bf76a2b46a4b).
    # This refactoring broke VMTK's vtkvmtkStaticTemporalInterpolatedVelocityField class.
    # Until VMTK is updated to use the new API, VTK_VMTK_BUILD_STREAMTRACER is not available anymore.
    message(FATAL_ERROR "VTK_VMTK_BUILD_STREAMTRACER is not yet supported for VTK version 9.2 and above.")
  endif()

  set (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkStaticTemporalInterpolatedVelocityField.cxx vtkvmtkStaticTemporalStreamTracer.cxx)
endif ()

vmtk_build_library(
  NAME Misc
  SRCS ${VTK_VMTK_MISC_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES}
  )
