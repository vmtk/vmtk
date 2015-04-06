set(VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES vtkvmtkMisc)

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_CONTRIB_COMPONENTS
    vtkCommonCore
    vtkFiltersGeometry
    vtkFiltersModeling
    vtkIOLegacy
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_CONTRIB_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_CONTRIB_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkHybrid
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

SET (VTK_VMTK_CONTRIB_SRCS
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
  NAME IO
  SRCS ${VTK_VMTK_CONTRIB_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES}
  )
