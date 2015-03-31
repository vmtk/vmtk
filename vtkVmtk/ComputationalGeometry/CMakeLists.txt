set(VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES )

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS
    vtkFiltersGeneral
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_COMPUTATIONALGEOMETRY_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkHybrid
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

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

set(lib_name vtkvmtkComputationalGeometry)

ADD_LIBRARY (${lib_name} ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
TARGET_LINK_LIBRARIES(${lib_name} vtkvmtkCommon ${VTK_VMTK_COMPUTATIONALGEOMETRY_TARGET_LINK_LIBRARIES})

INSTALL(TARGETS ${lib_name}
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/ComputationalGeometry/*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${VTK_VMTK_COMPUTATIONALGEOMETRY_SRCS}
    KIT_PYTHON_LIBRARIES vtkvmtkCommonPythonD
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
