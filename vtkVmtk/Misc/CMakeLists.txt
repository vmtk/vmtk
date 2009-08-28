SET (VTK_VMTK_MISC_SRCS
  vtkvmtkAppendFilter.cxx
  vtkvmtkBoundaryLayerGenerator.cxx
  vtkvmtkCurvedMPRImageFilter.cxx
  vtkvmtkImageBoxPainter.cxx
  vtkvmtkLevelSetSigmoidFilter.cxx
  vtkvmtkLinearizeMeshFilter.cxx
  vtkvmtkLinearToQuadraticMeshFilter.cxx
  vtkvmtkLinearToQuadraticSurfaceMeshFilter.cxx
  vtkvmtkMeshLambda2.cxx
  vtkvmtkMeshProjection.cxx
  vtkvmtkMeshVelocityStatistics.cxx
  vtkvmtkMeshVorticity.cxx
  vtkvmtkMeshWallShearRate.cxx
  vtkvmtkPolyDataNetworkExtraction.cxx
  vtkvmtkPolyDataKiteRemovalFilter.cxx
  vtkvmtkPolyDataSizingFunction.cxx
  vtkvmtkPolyDataToUnstructuredGridFilter.cxx
  vtkvmtkRBFInterpolation.cxx
  vtkvmtkSimpleCapPolyData.cxx
  vtkvmtkSmoothCapPolyData.cxx
  vtkvmtkStreamlineClusteringFilter.cxx
  vtkvmtkStreamlineOsculatingCentersFilter.cxx
  vtkvmtkStreamlineToParticlesFilter.cxx
  vtkvmtkSurfaceDistance.cxx
  vtkvmtkSurfaceProjection.cxx
  )

SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES vtkvmtkMisc vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkHybrid)

IF (VTK_VMTK_BUILD_TETGEN)
  SET (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkTetGenWrapper.cxx)

  INCLUDE_DIRECTORIES(${TETGEN_SOURCE_DIR})
  LINK_DIRECTORIES(${TETGEN_LIB_DIR})

  SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} tet)
ENDIF (VTK_VMTK_BUILD_TETGEN)

ADD_LIBRARY (vtkvmtkMisc ${VTK_VMTK_MISC_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkMisc ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES})

INSTALL(TARGETS vtkvmtkMisc
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
COMPONENT RuntimeLibraries
)

#FILE(GLOB files "${VTK_VMTK_MISC_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Misc/*.h")
INSTALL(FILES ${files} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS vtkvmtkMisc)

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###



IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkMiscPython VTK_VMTK_MISC_PYTHON_SRCS "${VTK_VMTK_MISC_SRCS}")
  ADD_LIBRARY(vtkvmtkMiscPythonD ${VTK_VMTK_MISC_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkMiscPython MODULE vtkvmtkMiscPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkMiscPythonD vtkvmtkMisc vtkvmtkDifferentialGeometry vtkvmtkDifferentialGeometryPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkMiscPython vtkvmtkMiscPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkMiscPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkMiscPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )
  INSTALL(TARGETS vtkvmtkMiscPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkMiscTCL VTK_VMTK_MISC_TCL_SRCS "${VTK_VMTK_MISC_SRCS}" "")
  ADD_LIBRARY(vtkvmtkMiscTCL ${VTK_VMTK_MISC_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkMiscTCL vtkvmtkMisc vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)

  INSTALL(TARGETS vtkvmtkMiscTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )

  ### START VMTK IN SLICER PATCH ###
  IF (VTK_VMTK_IN_SLICER)
    SET(LIBS vtkvmtkMiscTCL)

    FOREACH(LIB ${LIBS})
      slicer3_set_modules_output_path(${LIB})
    ENDFOREACH(LIB ${LIBS})
    
    slicer3_install_modules(${LIBS})
  ENDIF (VTK_VMTK_IN_SLICER)
  ### END VMTK IN SLICER PATCH ###

ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
