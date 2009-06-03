SET (VTK_VMTK_CONTRIB_SRCS
  )

SET (VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES vtkvmtkContrib vtkvmtkDifferentialGeometry vtkCommon vtkFiltering vtkGraphics vtkHybrid)

ADD_LIBRARY (vtkvmtkContrib ${VTK_VMTK_CONTRIB_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkContrib ${VTK_VMTK_CONTRIB_TARGET_LINK_LIBRARIES})
SET_TARGET_PROPERTIES(vtkvmtkContrib PROPERTIES LINKER_LANGUAGE CXX)

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkContrib)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_CONTRIB_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkContribPython VTK_VMTK_CONTRIB_PYTHON_SRCS "${VTK_VMTK_CONTRIB_SRCS}")
  ADD_LIBRARY(vtkvmtkContribPythonD ${VTK_VMTK_CONTRIB_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkContribPython MODULE vtkvmtkContribPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkContribPythonD vtkvmtkContrib vtkvmtkDifferentialGeometry vtkvmtkDifferentialGeometryPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkContribPython vtkvmtkContribPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkContribPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkContribPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkContribPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkContribTCL VTK_VMTK_MISC_TCL_SRCS "${VTK_VMTK_CONTRIB_SRCS}" "")
  ADD_LIBRARY(vtkvmtkContribTCL ${VTK_VMTK_CONTRIB_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkContribTCL vtkvmtkContrib vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkContribTCL)
ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
