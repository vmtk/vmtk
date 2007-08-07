SUBDIRS(
  Doxygen
  laspack
#  tetgen1.4.1
  tetgen1.4.2
  )

IF (NOT VMTK_EXTERNAL_vtkITK)
  SUBDIRS(vtkITK)
ENDIF (NOT VMTK_EXTERNAL_vtkITK)
