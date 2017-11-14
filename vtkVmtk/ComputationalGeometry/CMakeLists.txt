set(VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES vtkvmtkCommon)

set( VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS
vtkFiltersGeneral
)
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
list(APPEND VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS
  vtkWrappingPythonCore
  )
endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS} REQUIRED )
include( ${VTK_USE_FILE} )
list( APPEND VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

SET (VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS
  vtkvmtkAppendFilter.cxx
  vtkvmtkCapPolyData.cxx
  vtkvmtkCenterlineAttributesFilter.cxx
  vtkvmtkCenterlineBifurcationVectors.cxx
  vtkvmtkCenterlineBifurcationReferenceSystems.cxx
  vtkvmtkCenterlineBranchExtractor.cxx
  vtkvmtkCenterlineBranchGeometry.cxx
  vtkvmtkCenterlineEndpointExtractor.cxx
  vtkvmtkCenterlineGeometry.cxx
  vtkvmtkCenterlineReferenceSystemAttributesOffset.cxx
  vtkvmtkCenterlineSmoothing.cxx
  vtkvmtkCenterlineSphereDistance.cxx
  vtkvmtkCenterlineSplitExtractor.cxx
  vtkvmtkCenterlineSplittingAndGroupingFilter.cxx
  vtkvmtkCenterlineUtilities.cxx
  vtkvmtkBoundaryReferenceSystems.cxx
  vtkvmtkInternalTetrahedraExtractor.cxx
  vtkvmtkMergeCenterlines.cxx
  vtkvmtkMinHeap.cxx
  vtkvmtkNonManifoldFastMarching.cxx
  vtkvmtkNonManifoldSteepestDescent.cxx
  vtkvmtkPolyBall.cxx
  vtkvmtkPolyBallLine.cxx
  vtkvmtkPolyBallModeller.cxx
  vtkvmtkPolyDataBifurcationSections.cxx
  vtkvmtkPolyDataBifurcationProfiles.cxx
  vtkvmtkPolyDataBoundaryExtractor.cxx
  vtkvmtkPolyDataBranchSections.cxx
  vtkvmtkPolyDataBranchUtilities.cxx
  vtkvmtkPolyDataCenterlines.cxx
  vtkvmtkPolyDataCenterlineGroupsClipper.cxx
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter.cxx
  vtkvmtkPolyDataCenterlineAngularMetricFilter.cxx
  vtkvmtkPolyDataCenterlineMetricFilter.cxx
  vtkvmtkPolyDataCenterlineProjection.cxx
  vtkvmtkPolyDataCenterlineSections.cxx
  vtkvmtkPolyDataFlowExtensionsFilter.cxx
  vtkvmtkPolyDataDistanceToCenterlines.cxx
  vtkvmtkPolyDataLineEmbedder.cxx
  vtkvmtkPolyDataLocalGeometry.cxx
  vtkvmtkPolyDataPatchingFilter.cxx
  vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter.cxx
  vtkvmtkPolyDataScissors.cxx
  vtkvmtkPolyDataStretchMappingFilter.cxx
  vtkvmtkReferenceSystemUtilities.cxx
  vtkvmtkSimplifyVoronoiDiagram.cxx
  vtkvmtkSteepestDescentLineTracer.cxx
  vtkvmtkSteepestDescentShooter.cxx
  vtkvmtkUnstructuredGridCenterlineGroupsClipper.cxx
  vtkvmtkUnstructuredGridCenterlineSections.cxx
  vtkvmtkVoronoiDiagram3D.cxx
  )

SET_SOURCE_FILES_PROPERTIES (
  vtkvmtkCenterlineSplittingAndGroupingFilter.cxx
  vtkvmtkPolyDataCenterlineMetricFilter.cxx
  ABSTRACT
)

vmtk_build_library(
  NAME ComputationalGeometry
  SRCS ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES}
  )
