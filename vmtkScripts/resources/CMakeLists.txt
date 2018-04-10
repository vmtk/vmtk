PROJECT(VMTKSCRIPT_RESOURCES)

SET(VMTKSCRIPT_RESOURCES_SRCS
  vmtkimagevolumeviewerpresets.xml
  )

install(FILES ${VMTKSCRIPT_RESOURCES_SRCS}
  DESTINATION lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk/share
  COMPONENT PythonRuntimeLibraries)