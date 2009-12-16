SET (VTK_VMTK_CONTRIB_SRCS
  vtkvmtkBoundaryLayerGenerator2.cxx
  vtkvmtkCellDimensionFilter.cxx
  vtkvmtkCenterlineInterpolateArray.cxx
  vtkvmtkDolfinWriter2.cxx
  vtkvmtkPolyBallLine2.cxx
  vtkvmtkPolyDataDijkstraDistanceToPoints.cxx
  vtkvmtkPolyDataDistanceToSpheres.cxx
  vtkvmtkPolyDataGeodesicRBFInterpolation.cxx
  vtkvmtkPolyDataSampleFunction.cxx
  vtkvmtkRBFInterpolation2.cxx
  vtkvmtkSurfaceProjectCellArray.cxx
  )

SET (VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES vtkvmtkContrib vtkvmtkMisc vtkCommon vtkFiltering vtkGraphics vtkHybrid)

ADD_LIBRARY (vtkvmtkContrib ${VTK_VMTK_CONTRIB_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkContrib ${VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES})
SET_TARGET_PROPERTIES(vtkvmtkContrib PROPERTIES LINKER_LANGUAGE CXX)

INSTALL(TARGETS vtkvmtkContrib
LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
COMPONENT RuntimeLibraries
)

#FILE(GLOB files "${VTK_VMTK_CONTRIB_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Contrib/*.h")
INSTALL(FILES ${files} DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR})

### START VMTK IN SLICER PATCH ###
IF (VTK_VMTK_IN_SLICER)
  SET(LIBS vtkvmtkContrib)

  FOREACH(LIB ${LIBS})
    slicer3_set_modules_output_path(${LIB})
  ENDFOREACH(LIB ${LIBS})
    
  slicer3_install_modules(${LIBS})
ENDIF (VTK_VMTK_IN_SLICER)
### END VMTK IN SLICER PATCH ###


IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkContribPython VTK_VMTK_CONTRIB_PYTHON_SRCS "${VTK_VMTK_CONTRIB_SRCS}")
  ADD_LIBRARY(vtkvmtkContribPythonD ${VTK_VMTK_CONTRIB_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkContribPython MODULE vtkvmtkContribPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkContribPythonD vtkvmtkContrib vtkvmtkMisc vtkvmtkMiscPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkContribPython vtkvmtkContribPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkContribPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL(TARGETS vtkvmtkContribPythonD
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )
  INSTALL(TARGETS vtkvmtkContribPython
    LIBRARY DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  )
  
  CONFIGURE_FILE(${VTK_VMTK_SOURCE_DIR}/Contrib/vtkvmtkcontrib.py ${VTK_VMTK_BINARY_DIR}/Contrib/vtkvmtkcontrib.py COPYONLY)
  #INSTALL_FILES(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} FILES ${VTK_VMTK_BINARY_DIR}/vtkvmtk.py)
  INSTALL(FILES ${VTK_VMTK_BINARY_DIR}/Contrib/vtkvmtkcontrib.py DESTINATION ${VTK_VMTK_MODULE_INSTALL_LIB_DIR})
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkContribTCL VTK_VMTK_CONTRIB_TCL_SRCS "${VTK_VMTK_CONTRIB_SRCS}" "")
  ADD_LIBRARY(vtkvmtkContribTCL ${VTK_VMTK_CONTRIB_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkContribTCL vtkvmtkContrib vtkvmtkMiscTCL vtkvmtkMisc vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)
  INSTALL(TARGETS vtkvmtkContribTCL
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} 
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} 
    COMPONENT RuntimeLibraries
    )

  ### START VMTK IN SLICER PATCH ###
  IF (VTK_VMTK_IN_SLICER)
    SET(LIBS vtkvmtkContribTCL)

    FOREACH(LIB ${LIBS})
      slicer3_set_modules_output_path(${LIB})
    ENDFOREACH(LIB ${LIBS})
    
    slicer3_install_modules(${LIBS})
  ENDIF (VTK_VMTK_IN_SLICER)
  ### END VMTK IN SLICER PATCH ###

ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
