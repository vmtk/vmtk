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
  vtkvmtkPolyDataCylinderHarmonicMappingFilter.cxx
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
  vtkvmtkPolyDataStencilFlowFilter.cxx
  vtkvmtkPolyDataUmbrellaStencil.cxx
  vtkvmtkSparseMatrix.cxx
  vtkvmtkSparseMatrixRow.cxx
  vtkvmtkStencil.cxx
  vtkvmtkStencils.cxx
  vtkvmtkUnstructuredGridFEGradientAssembler.cxx
  vtkvmtkUnstructuredGridFELaplaceAssembler.cxx
  vtkvmtkUnstructuredGridGradientFilter.cxx
  vtkvmtkUnstructuredGridHarmonicMappingFilter.cxx
  vtkvmtkUnstructuredGridNeighborhood.cxx
  )

INCLUDE_DIRECTORIES(${LASPACK_SOURCE_DIR})
LINK_DIRECTORIES(${LASPACK_LIB_DIR})

ADD_LIBRARY (vtkvmtkDifferentialGeometry ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkvmtkComputationalGeometry laspack)
  
INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkDifferentialGeometry)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkDifferentialGeometryPython VTK_VMTK_DIFFERENTIALGEOMETRY_PYTHON_SRCS "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}")
  ADD_LIBRARY(vtkvmtkDifferentialGeometryPythonD ${VTK_VMTK_DIFFERENTIALGEOMETRY_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkDifferentialGeometryPython MODULE vtkvmtkDifferentialGeometryPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometryPythonD vtkvmtkDifferentialGeometry vtkvmtkComputationalGeometryPythonD vtkCommonPythonD vtkFilteringPythonD vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkDifferentialGeometryPython vtkvmtkDifferentialGeometryPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkDifferentialGeometryPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkDifferentialGeometryPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkDifferentialGeometryPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

