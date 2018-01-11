set(VTK_VMTK_IO_TARGET_LINK_LIBRARIES )

set( VTK_VMTK_IO_COMPONENTS
vtkCommonCore
vtkIOImage
vtkIOLegacy
vtkImagingCore
)
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
list(APPEND VTK_VMTK_IO_COMPONENTS
  vtkWrappingPythonCore
  )
endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_IO_COMPONENTS} REQUIRED )
include( ${VTK_USE_FILE} )
list( APPEND VTK_VMTK_IO_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )


SET (VTK_VMTK_IO_SRCS
  vtkvmtkDolfinWriter.cxx
  vtkvmtkFDNEUTReader.cxx
  vtkvmtkFDNEUTWriter.cxx
  vtkvmtkFluentWriter.cxx
  vtkvmtkPNGWriter.cxx
  vtkvmtkTetGenReader.cxx
  vtkvmtkTetGenWriter.cxx
  vtkvmtkXdaReader.cxx
  vtkvmtkXdaWriter.cxx
  )

# XXX Ensure DICOMParser directory provided by VTK is included before
#     the one provided by ITK.
include_directories(BEFORE ${vtkDICOMParser_INCLUDE_DIRS})

vmtk_build_library(
  NAME IO
  SRCS ${VTK_VMTK_IO_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_IO_TARGET_LINK_LIBRARIES}
  )
