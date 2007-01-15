SET (VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS
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
  vtkvmtkPolyDataBoundaryExtractor.cxx
  vtkvmtkPolyDataBranchSections.cxx
  vtkvmtkPolyDataBranchUtilities.cxx
  vtkvmtkPolyDataCenterlines.cxx
  vtkvmtkPolyDataCenterlineGroupsClipper.cxx
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter.cxx
  vtkvmtkPolyDataCenterlineAngularMetricFilter.cxx
  vtkvmtkPolyDataCenterlineMetricFilter.cxx
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
  vtkvmtkVoronoiDiagram3D.cxx
  )

ADD_LIBRARY (vtkvmtkComputationalGeometry ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkComputationalGeometry vtkvmtkCommon vtkCommon vtkFiltering vtkGraphics vtkHybrid)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkComputationalGeometry)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkComputationalGeometryPython VTK_VMTK_COMPUTATIONALGEOMETRY_PYTHON_SRCS "${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}")
  ADD_LIBRARY(vtkvmtkComputationalGeometryPythonD ${VTK_VMTK_COMPUTATIONALGEOMETRY_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkComputationalGeometryPython MODULE vtkvmtkComputationalGeometryPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkComputationalGeometryPythonD vtkvmtkComputationalGeometry vtkvmtkCommon vtkvmtkCommonPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD vtkHybrid vtkHybridPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkComputationalGeometryPython vtkvmtkComputationalGeometryPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkComputationalGeometryPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkComputationalGeometryPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
