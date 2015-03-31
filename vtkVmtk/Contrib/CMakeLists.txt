set(VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES vtkvmtkMisc)

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_CONTRIB_COMPONENTS
    vtkCommonCore
    vtkFiltersGeometry
    vtkFiltersModeling
    vtkIOLegacy
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_CONTRIB_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_CONTRIB_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkHybrid
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

SET (VTK_VMTK_CONTRIB_SRCS
  vtkvmtkBoundaryLayerGenerator2.cxx
  vtkvmtkCellDimensionFilter.cxx
  vtkvmtkCenterlineInterpolateArray.cxx
  vtkvmtkConcaveAnnularCapPolyData.cxx
  vtkvmtkDolfinWriter2.cxx
  vtkvmtkPolyBallLine2.cxx
  vtkvmtkPolyDataDijkstraDistanceToPoints.cxx
  vtkvmtkPolyDataDistanceToSpheres.cxx
  vtkvmtkPolyDataGeodesicRBFInterpolation.cxx
  vtkvmtkPolyDataSampleFunction.cxx
  vtkvmtkRBFInterpolation2.cxx
  vtkvmtkSurfaceProjectCellArray.cxx
  )


set(lib_name vtkvmtkContrib)

ADD_LIBRARY (${lib_name} ${VTK_VMTK_CONTRIB_SRCS})
TARGET_LINK_LIBRARIES(${lib_name} ${VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES})
SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)

INSTALL(TARGETS ${lib_name}
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${VTK_VMTK_CONTRIB_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Contrib/*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${VTK_VMTK_CONTRIB_SRCS}
    KIT_PYTHON_LIBRARIES vtkvmtkMiscPythonD
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
