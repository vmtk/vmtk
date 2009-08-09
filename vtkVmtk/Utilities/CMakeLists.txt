SUBDIRS(
  Doxygen
  laspack
  vtkvmtkITK
  )

IF (VTK_VMTK_BUILD_TETGEN)
  SUBDIRS(tetgen1.4.2)
ENDIF (VTK_VMTK_BUILD_TETGEN)

