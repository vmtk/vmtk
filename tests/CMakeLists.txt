PROJECT(VMTK_TESTING)

include(ExternalData)

set(ExternalData_URL_TEMPLATES
    "https://data.kitware.com:443/api/v1/file/hashsum/%(algo)/%(hash)/download"
 )


set(VMTK_TESTING_DIRECTORIES
  test_PypeS
  test_vmtkScripts
  )

FOREACH (TEST_DIR ${VMTK_TESTING_DIRECTORIES})
  add_subdirectory(${TEST_DIR})
ENDFOREACH (TEST_DIR)


set(VMTK_TESTING_INIT_SRC
  __init__.py
  )

IF(NOT VMTK_TESTING_INSTALL_LIB_DIR)
  SET(VMTK_TESTING_INSTALL_LIB_DIR tests)
ENDIF(NOT VMTK_TESTING_INSTALL_LIB_DIR)

INSTALL(FILES ${VMTK_TESTING_INIT_SRC}
  DESTINATION ${VMTK_TESTING_INSTALL_LIB_DIR}
  COMPONENT PythonRuntimeLibraries)

ExternalData_Add_Test(MyData
  NAME MyTest
  COMMAND python -m pytest ${VMTK_TESTING_INSTALL_LIB_DIR}
  DATA{data/,RECURSE:,REGEX:.*}
  )
ExternalData_Add_Target(MyData)