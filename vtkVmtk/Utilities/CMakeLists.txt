SUBDIRS(
  Doxygen
  OpenNL
  #Stellar_1.0
  vtkvmtkITK
  )

IF (VTK_VMTK_BUILD_TETGEN)
  SUBDIRS(tetgen1.4.3)
ENDIF (VTK_VMTK_BUILD_TETGEN)

