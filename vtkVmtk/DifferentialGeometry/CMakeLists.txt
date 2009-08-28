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
  vtkvmtkLASPACKLinearSystemSolver.cxx
  vtkvmtkLinearSystem.cxx
  vtkvmtkLinearSystemSolver.cxx
  vtkvmtkNeighborhood.cxx
  vtkvmtkNeighborhoods.cxx
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

#INCLUDE_DIRECTORIES(${LASPACK_SOURCE_DIR})
#LINK_DIRECTORIES(${LASPACK_LIB_DIR})
INCLUDE_DIRECTORIES(${VTK_VMTK_SOURCE_DIR}/Utilities/laspack)

ADD_LIBRARY (vtkvmtkDifferentialGeometry ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkvmtkComputationalGeometry laspack)
  
INSTALL(TARGETS vtkvmtkDifferentialGeometry
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
COMPONENT RuntimeLibraries
)

#FILE(GLOB files "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/DifferentialGeometry/*.h")
INSTALL(FILES ${files} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

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
  ADD_LIBRARY(vtkvmtkDifferentialGeometryPython MODULE vtkvmtkDifferentialGeometryPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometryPythonD vtkvmtkDifferentialGeometry vtkvmtkComputationalGeometryPythonD vtkCommonPythonD vtkFilteringPythonD vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkDifferentialGeometryPython vtkvmtkDifferentialGeometryPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkDifferentialGeometryPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkDifferentialGeometryPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )
  INSTALL(TARGETS vtkvmtkDifferentialGeometryPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkDifferentialGeometryTCL VTK_VMTK_DIFFERENTIALGEOMETRY_TCL_SRCS "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}" "")
  ADD_LIBRARY(vtkvmtkDifferentialGeometryTCL ${VTK_VMTK_DIFFERENTIALGEOMETRY_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkvmtkComputationalGeometryTCL vtkvmtkComputationalGeometry vtkvmtkCommon vtkvmtkCommonTCL vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)

  INSTALL(TARGETS vtkvmtkDifferentialGeometryTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )

  ### START VMTK IN SLICER PATCH ###
  IF (VTK_VMTK_IN_SLICER)
    SET(LIBS vtkvmtkDifferentialGeometryTCL)

    FOREACH(LIB ${LIBS})
      slicer3_set_modules_output_path(${LIB})
    ENDFOREACH(LIB ${LIBS})
    
    slicer3_install_modules(${LIBS})
  ENDIF (VTK_VMTK_IN_SLICER)
  ### END VMTK IN SLICER PATCH ###

ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
