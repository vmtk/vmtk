set(VTK_VMTK_IO_TARGET_LINK_LIBRARIES )

set( VTK_VMTK_IO_COMPONENTS
  ${VTK_COMPONENT_PREFIX}CommonCore
  ${VTK_COMPONENT_PREFIX}CommonDataModel
  ${VTK_COMPONENT_PREFIX}CommonMisc
  ${VTK_COMPONENT_PREFIX}IOImage
  ${VTK_COMPONENT_PREFIX}IOLegacy
  ${VTK_COMPONENT_PREFIX}ImagingCore
  )
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND VTK_VMTK_IO_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_IO_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND VTK_VMTK_IO_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )


set (VTK_VMTK_IO_SRCS
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
