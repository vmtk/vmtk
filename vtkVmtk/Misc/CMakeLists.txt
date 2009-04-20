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

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMisc)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_MISC_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkMiscPython VTK_VMTK_MISC_PYTHON_SRCS "${VTK_VMTK_MISC_SRCS}")
  ADD_LIBRARY(vtkvmtkMiscPythonD ${VTK_VMTK_MISC_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkMiscPython MODULE vtkvmtkMiscPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkMiscPythonD vtkvmtkMisc vtkvmtkDifferentialGeometry vtkvmtkDifferentialGeometryPythonD vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkMiscPython vtkvmtkMiscPythonD)
  IF(WIN32 AND NOT CYGWIN)
     SET_TARGET_PROPERTIES(vtkvmtkMiscPython PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMiscPythonD)
  INSTALL_TARGETS(${VTK_VMTK_MODULE_INSTALL_LIB_DIR} vtkvmtkMiscPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

IF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
  SET(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/Tcl/hints)
  VTK_WRAP_TCL3(vtkvmtkMiscTCL VTK_VMTK_MISC_TCL_SRCS "${VTK_VMTK_MISC_SRCS}" "")
  ADD_LIBRARY(vtkvmtkMiscTCL ${VTK_VMTK_MISC_TCL_SRCS})
  TARGET_LINK_LIBRARIES(vtkvmtkMiscTCL vtkvmtkMisc vtkvmtkDifferentialGeometryTCL vtkvmtkDifferentialGeometry vtkCommon vtkCommonTCL vtkFiltering vtkFilteringTCL vtkGraphics vtkGraphicsTCL)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMiscTCL)
ENDIF (VTK_WRAP_TCL AND VTK_VMTK_WRAP_TCL)
