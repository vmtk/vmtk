SUBDIRS(
  Doxygen
  laspack
  tetgen1.4.1
  )

IF (NOT VMTK_EXTERNAL_vtkITK)
  SUBDIRS(vtkITK)
ENDIF (NOT VMTK_EXTERNAL_vtkITK)
