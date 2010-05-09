SUBDIRS(
  Doxygen
  laspack
  OpenNL
  vtkvmtkITK
  )

IF (VTK_VMTK_BUILD_TETGEN)
  SUBDIRS(tetgen1.4.3)
ENDIF (VTK_VMTK_BUILD_TETGEN)

