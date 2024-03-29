set(VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES vtkvmtkMisc)

set( VTK_VMTK_CONTRIB_COMPONENTS
  ${VTK_COMPONENT_PREFIX}CommonCore
  ${VTK_COMPONENT_PREFIX}FiltersGeometry
  ${VTK_COMPONENT_PREFIX}FiltersModeling
  ${VTK_COMPONENT_PREFIX}IOLegacy
  )

if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND VTK_VMTK_CONTRIB_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()

find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_CONTRIB_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

# if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
#   list(APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES
#     vtkvmtkMiscPythonD
#     )
# endif()

set (VTK_VMTK_CONTRIB_SRCS
  vtkvmtkBoundaryLayerGenerator2.cxx
  vtkvmtkCellDimensionFilter.cxx
  vtkvmtkCenterlineInterpolateArray.cxx
  vtkvmtkConcaveAnnularCapPolyData.cxx
  vtkvmtkDolfinWriter2.cxx
  vtkvmtkPolyBallLine2.cxx
  vtkvmtkPolyDataDijkstraDistanceToPoints.cxx
  vtkvmtkPolyDataDistanceToSpheres.cxx
  vtkvmtkPolyDataGeodesicRBFInterpolation.cxx
  vtkvmtkPolyDataSampleFunction.cxx
  vtkvmtkRBFInterpolation2.cxx
  vtkvmtkSurfaceProjectCellArray.cxx
  )

vmtk_build_library(
  NAME Contrib
  SRCS ${VTK_VMTK_CONTRIB_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES}
  )
