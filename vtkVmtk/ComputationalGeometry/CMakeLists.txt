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
  vtkvmtkUnstructuredGridCenterlineSections.cxx
  vtkvmtkVoronoiDiagram3D.cxx
  )

SET_SOURCE_FILES_PROPERTIES (
  vtkvmtkCenterlineSplittingAndGroupingFilter.cxx
  vtkvmtkPolyDataCenterlineMetricFilter.cxx
  ABSTRACT
)

ADD_LIBRARY (vtkvmtkComputationalGeometry ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkComputationalGeometry vtkvmtkCommon vtkCommon vtkFiltering vtkGraphics vtkHybrid)

INSTALL(TARGETS vtkvmtkComputationalGeometry
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
COMPONENT RuntimeLibraries
)

#FILE(GLOB files "${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/ComputationalGeometry/*.h")
INSTALL(FILES ${files} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS vtkvmtkComputationalGeometry)

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###



IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkComputationalGeometryPython VTK_VMTK_COMPUTATIONALGEOMETRY_PYTHON_SRCS "${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}")
  ADD_LIBRARY(vtkvmtkComputationalGeometryPythonD ${VTK_VMTK_COMPUTATIONALGEOMETRY_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkComputationalGeometryPython MODULE vtkvmtkComputationalGeometryPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkComputationalGeometryPythonD vtkvmtkComputationalGeometry vtkvmtkCommon vtkvmtkCommonPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD vtkHybrid vtkHybridPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkComputationalGeometryPython vtkvmtkComputationalGeometryPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkComputationalGeometryPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkComputationalGeometryPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
  INSTALL(TARGETS vtkvmtkComputationalGeometryPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkComputationalGeometryTCL VTK_VMTK_COMPUTATIONALGEOMETRY_TCL_SRCS "${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}" "")
  ADD_LIBRARY(vtkvmtkComputationalGeometryTCL ${VTK_VMTK_COMPUTATIONALGEOMETRY_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkComputationalGeometryTCL vtkvmtkComputationalGeometry vtkvmtkCommon vtkvmtkCommonTCL vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL vtkHybrid vtkHybridTCL)

  INSTALL(TARGETS vtkvmtkComputationalGeometryTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )

  ### START VMTK IN SLICER PATCH ###
  IF (VTK_VMTK_IN_SLICER)
    SET(LIBS vtkvmtkComputationalGeometryTCL)

    FOREACH(LIB ${LIBS})
      slicer3_set_modules_output_path(${LIB})
    ENDFOREACH(LIB ${LIBS})
    
    slicer3_install_modules(${LIBS})
  ENDIF (VTK_VMTK_IN_SLICER)
  ### END VMTK IN SLICER PATCH ###

ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
