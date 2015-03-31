set(VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES vtkvmtkComputationalGeometry nl)

if( ${VTK_VERSION_MAJOR} GREATER 5 )
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
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_DIFFERENTIALGEOMETRY_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

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

set(lib_name vtkvmtkDifferentialGeometry)

ADD_LIBRARY (${lib_name} ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
TARGET_LINK_LIBRARIES(${lib_name} ${VTK_VMTK_DIFFERENTIALGEOMETRY_TARGET_LINK_LIBRARIES})
  
INSTALL(TARGETS ${lib_name}
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
  SET(LIBS ${lib_name})

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###




IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${VTK_VMTK_DIFFERENTIALGEOMETRY_SRCS}
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
