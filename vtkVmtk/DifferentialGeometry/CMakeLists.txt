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

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkDifferentialGeometryTCL VTK_VMTK_DIFFERENTIALGEOMETRY_TCL_SRCS "${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}" "")
  ADD_LIBRARY(vtkvmtkDifferentialGeometryTCL ${VTK_VMTK_DIFFERENTIALGEOMETRY_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkvmtkComputationalGeometryTCL vtkvmtkComputationalGeometry vtkvmtkCommon vtkvmtkCommonTCL vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkDifferentialGeometryTCL)
ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
