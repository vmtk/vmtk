set(VTK_VMTK_COMMON_TARGET_LINK_LIBRARIES )

set(VTK_VMTK_COMMON_COMPONENTS
  ${VTK_COMPONENT_PREFIX}CommonCore
  )
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND VTK_VMTK_COMMON_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()

find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${VTK_VMTK_COMMON_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND VTK_VMTK_COMMON_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

set (VTK_VMTK_COMMON_SRCS
  vtkvmtkMath.cxx
  )

vmtk_build_library(
  NAME Common
  SRCS ${VTK_VMTK_COMMON_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_COMMON_TARGET_LINK_LIBRARIES}
  )
