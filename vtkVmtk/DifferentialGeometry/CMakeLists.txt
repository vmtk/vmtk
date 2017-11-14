set(VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES vtkvmtkComputationalGeometry nl)

set( VTK_VMTK_DIFFERENTIALGEOMETRY_COMPONENTS
vtkCommonCore
vtkFiltersGeneral
vtkFiltersVerdict
)
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
list(APPEND VTK_VMTK_DIFFERENTIALGEOMETRY_COMPONENTS
  vtkWrappingPythonCore
  )
endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_DIFFERENTIALGEOMETRY_COMPONENTS} REQUIRED )
include( ${VTK_USE_FILE} )
list( APPEND VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

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

vmtk_build_library(
  NAME DifferentialGeometry
  SRCS ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES}
  )
