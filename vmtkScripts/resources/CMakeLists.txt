PROJECT(VMTKSCRIPT_RESOURCES)

SET(VMTKSCRIPT_RESOURCES_SRCS
  vmtkimagevolumeviewerpresets.xml
  )

if(WIN32)
  install(FILES ${VMTKSCRIPT_RESOURCES_SRCS}
    DESTINATION lib/site-packages/vmtk/share
    COMPONENT PythonRuntimeLibraries)
else(WIN32)
  install(FILES ${VMTKSCRIPT_RESOURCES_SRCS}
    DESTINATION lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk/share
    COMPONENT PythonRuntimeLibraries)
endif(WIN32)