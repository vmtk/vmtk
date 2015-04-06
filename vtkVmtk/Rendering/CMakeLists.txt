set(VTK_VMTK_RENDERING_TARGET_LINK_LIBRARIES )

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_RENDERING_COMPONENTS
    vtkCommonCore
    vtkInteractionWidgets
    vtkRenderingOpenGL
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_RENDERING_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_RENDERING_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_RENDERING_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_RENDERING_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkGraphics
    vtkHybrid
    vtkRendering
    vtkWidgets
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

SET (VTK_VMTK_RENDERING_SRCS
  vtkvmtkImagePlaneWidget.cxx
  vtkvmtkInteractorStyleTrackballCamera.cxx
  )

vmtk_build_library(
  NAME Rendering
  SRCS ${VTK_VMTK_RENDERING_SRCS}
  TARGET_LINK_LIBRARIES ${VTK_VMTK_RENDERING_TARGET_LINK_LIBRARIES}
  )
