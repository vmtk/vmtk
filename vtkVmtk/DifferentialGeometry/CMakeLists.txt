SET (VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS
  vtkvmtkBoundaryConditions.cxx
  vtkvmtkDataSetItem.cxx
  vtkvmtkDataSetItems.cxx
  vtkvmtkDirichletBoundaryConditions.cxx
  vtkvmtkDoubleVector.cxx
  vtkvmtkEllipticProblem.cxx
  vtkvmtkEmptyNeighborhood.cxx
  vtkvmtkEmptyStencil.cxx
  vtkvmtkFEAssembler.cxx
  vtkvmtkFEShapeFunctions.cxx
  vtkvmtkGaussQuadrature.cxx
  vtkvmtkItem.cxx
  vtkvmtkItems.cxx
  vtkvmtkLinearSystem.cxx
  vtkvmtkLinearSystemSolver.cxx
  vtkvmtkNeighborhood.cxx
  vtkvmtkNeighborhoods.cxx
  vtkvmtkOpenNLLinearSystemSolver.cxx
  vtkvmtkPolyDataAreaWeightedUmbrellaStencil.cxx
  vtkvmtkPolyDataClampedSmoothingFilter.cxx
  vtkvmtkPolyDataCylinderHarmonicMappingFilter.cxx
  vtkvmtkPolyDataDiscreteElasticaFilter.cxx
  vtkvmtkPolyDataFEGradientAssembler.cxx
  vtkvmtkPolyDataFELaplaceAssembler.cxx
  vtkvmtkPolyDataFELaplaceBeltramiStencil.cxx
  vtkvmtkPolyDataFVFELaplaceBeltramiStencil.cxx
  vtkvmtkPolyDataGradientFilter.cxx
  vtkvmtkPolyDataGradientStencil.cxx
  vtkvmtkPolyDataHarmonicMappingFilter.cxx
  vtkvmtkPolyDataLaplaceBeltramiStencil.cxx
  vtkvmtkPolyDataManifoldExtendedNeighborhood.cxx
  vtkvmtkPolyDataManifoldNeighborhood.cxx
  vtkvmtkPolyDataManifoldStencil.cxx
  vtkvmtkPolyDataMeanCurvature.cxx
  vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter.cxx
  vtkvmtkPolyDataNeighborhood.cxx
  vtkvmtkPolyDataRigidSurfaceModelling.cxx
  vtkvmtkPolyDataStencilFlowFilter.cxx
  vtkvmtkPolyDataSurfaceRemeshing.cxx
  vtkvmtkPolyDataUmbrellaStencil.cxx
  vtkvmtkSparseMatrix.cxx
  vtkvmtkSparseMatrixRow.cxx
  vtkvmtkStencil.cxx
  vtkvmtkStencils.cxx
  vtkvmtkUnstructuredGridFEGradientAssembler.cxx
  vtkvmtkUnstructuredGridFELaplaceAssembler.cxx
  vtkvmtkUnstructuredGridFEVorticityAssembler.cxx
  vtkvmtkUnstructuredGridGradientFilter.cxx
  vtkvmtkUnstructuredGridVorticityFilter.cxx
  vtkvmtkUnstructuredGridHarmonicMappingFilter.cxx
  vtkvmtkUnstructuredGridNeighborhood.cxx
  )

SET_SOURCE_FILES_PROPERTIES (
  vtkvmtkBoundaryConditions.cxx
  vtkvmtkDataSetItem.cxx
  vtkvmtkDataSetItems.cxx
  vtkvmtkFEAssembler.cxx
  vtkvmtkItem.cxx
  vtkvmtkItems.cxx
  vtkvmtkNeighborhood.cxx
  vtkvmtkPolyDataLaplaceBeltramiStencil.cxx
  vtkvmtkPolyDataManifoldStencil.cxx
  vtkvmtkStencil.cxx
  ABSTRACT
)

INCLUDE_DIRECTORIES(${VTK_VMTK_SOURCE_DIR}/Utilities/OpenNL)

SET(VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES vtkCommon vtkFiltering vtkGraphics vtkvmtkComputationalGeometry nl)

ADD_LIBRARY (vtkvmtkDifferentialGeometry ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(vtkvmtkDifferentialGeometry PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometry ${VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES})
  
INSTALL(TARGETS vtkvmtkDifferentialGeometry
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/DifferentialGeometry/*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS vtkvmtkDifferentialGeometry)

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###




IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkDifferentialGeometryPython VTK_VMTK_DIFFERENTIALGEOMETRY_PYTHON_SRCS "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}")
  ADD_LIBRARY(vtkvmtkDifferentialGeometryPythonD ${VTK_VMTK_DIFFERENTIALGEOMETRY_PYTHON_SRCS})
  IF(VMTK_LIBRARY_PROPERTIES)
    SET_TARGET_PROPERTIES(vtkvmtkDifferentialGeometryPythonD PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
  ENDIF(VMTK_LIBRARY_PROPERTIES)
  ADD_LIBRARY(vtkvmtkDifferentialGeometryPython MODULE vtkvmtkDifferentialGeometryPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometryPythonD vtkvmtkDifferentialGeometry vtkvmtkComputationalGeometryPythonD vtkCommonPythonD vtkFilteringPythonD vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkDifferentialGeometryPython vtkvmtkDifferentialGeometryPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkDifferentialGeometryPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkDifferentialGeometryPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
    )
  INSTALL(TARGETS vtkvmtkDifferentialGeometryPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
