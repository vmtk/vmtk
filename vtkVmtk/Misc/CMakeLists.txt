SET (VTK_VMTK_MISC_SRCS
  vtkvmtkBoundaryLayerGenerator.cxx
  vtkvmtkImageBoxPainter.cxx
  vtkvmtkLinearToQuadraticMeshFilter.cxx
  vtkvmtkLinearToQuadraticSurfaceMeshFilter.cxx
  vtkvmtkMeshProjection.cxx
  vtkvmtkMeshVelocityStatistics.cxx
  vtkvmtkPolyDataBoundaryExtractor.cxx
  vtkvmtkPolyDataKiteRemovalFilter.cxx
  vtkvmtkPolyDataToUnstructuredGridFilter.cxx
  vtkvmtkSimpleCapPolyData.cxx
  vtkvmtkSurfaceDistance.cxx
  vtkvmtkSurfaceProjection.cxx
  )

INCLUDE_DIRECTORIES(${TETGEN_SOURCE_DIR})
LINK_DIRECTORIES(${TETGEN_LIB_DIR})

SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES vtkvmtkMisc vtkCommon vtkFiltering vtkGraphics tet)

#OPTION(VTK_VMTK_SURFMESH_SUPPORT "Build SurfMesh support." OFF)

#IF (VTK_VMTK_SURFMESH_SUPPORT)
#  FIND_PATH(VTK_VMTK_SURFMESH_INCLUDE_DIR SurfaceMesher.h /usr/include/surfmesh
#  /usr/local/include/surfmesh)
#  FIND_PATH(VTK_VMTK_SURFMESH_LIB_DIR libsurfmesh.a /usr/lib /usr/local/lib)
#  SET (VTK_VMTK_MISC_TARGET_LINK_LIBRARIES ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES} surfmesh)
#  SET (VTK_VMTK_MISC_SRCS ${VTK_VMTK_MISC_SRCS} vtkvmtkSurfMeshWrapper.cxx) 
#  INCLUDE_DIRECTORIES(${VTK_VMTK_SURFMESH_INCLUDE_DIR})
#  LINK_DIRECTORIES(${VTK_VMTK_SURFMESH_LIB_DIR})
#ENDIF (VTK_VMTK_SURFMESH_SUPPORT)

ADD_LIBRARY (vtkvmtkMisc ${VTK_VMTK_MISC_SRCS})
TARGET_LINK_LIBRARIES(vtkvmtkMisc ${VTK_VMTK_MISC_TARGET_LINK_LIBRARIES})

INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMisc)
INSTALL_FILES(${VTK_VMTK_INSTALL_INCLUDE_DIR} .h ${VTK_VMTK_MISC_SRCS})

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON3(vtkvmtkMiscPython VTK_VMTK_MISC_PYTHON_SRCS "${VTK_VMTK_MISC_SRCS}")
  ADD_LIBRARY(vtkvmtkMiscPythonD ${VTK_VMTK_MISC_PYTHON_SRCS})
  ADD_LIBRARY(vtkvmtkMiscPython MODULE vtkvmtkMiscPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkvmtkMiscPythonD vtkvmtkMisc vtkCommon vtkCommonPythonD vtkFiltering vtkFilteringPythonD vtkGraphics vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES (vtkvmtkMiscPython vtkvmtkMiscPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMiscPythonD)
  INSTALL_TARGETS(${VTK_VMTK_INSTALL_LIB_DIR} vtkvmtkMiscPython)
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
