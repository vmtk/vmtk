set(TEST_PYPES_SRCS
  test_pype.py
  test_pypes.py
  )

if(NOT TEST_PYPES_INSTALL_LIB_DIR)
  set(TEST_PYPES_INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/tests/test_PypeS)
endif()

foreach (SCRIPT_FILE ${TEST_PYPES_SRCS})
  configure_file(${VMTK_TESTING_SOURCE_DIR}/test_PypeS/${SCRIPT_FILE} ${TEST_PYPES_INSTALL_LIB_DIR}/${SCRIPT_FILE} COPYONLY)
endforeach ()