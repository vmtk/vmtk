project(VMTK_TESTING)

set(VMTK_TESTING_INIT_SRC
  __init__.py
  )

if(NOT VMTK_TESTING_INSTALL_LIB_DIR)
  set(VMTK_TESTING_INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/tests)
endif()

foreach (SCRIPT_FILE ${VMTK_TESTING_INIT_SRC})
  configure_file(${VMTK_TESTING_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_TESTING_INSTALL_LIB_DIR}/${SCRIPT_FILE} COPYONLY)
endforeach ()

# Check if submodule is clean
execute_process(
  COMMAND ${GIT_EXECUTABLE} status --porcelain ${VMTK_TESTING_SOURCE_DIR}/vmtk-test-data
  WORKING_DIRECTORY ${VMTK_TESTING_SOURCE_DIR}
  RESULT_VARIABLE error_code
  OUTPUT_VARIABLE repo_status
)
if(error_code)
  message(FATAL_ERROR "Failed to get the status")
endif()

string(LENGTH "${repo_status}" unclean_status)

# If not in clean state do not overwrite
if(unclean_status)
  message(WARNING "Test data repo in unclean state, will not overwrite test data")
else()
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init -- ${VMTK_TESTING_SOURCE_DIR}/vmtk-test-data
                  WORKING_DIRECTORY ${VMTK_TESTING_SOURCE_DIR})
endif()

if(NOT VMTK_TESTING_DATA_INSTALL_LIB_DIR)
  set(VMTK_TESTING_DATA_INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/tests)
endif()

file(COPY ${VMTK_TESTING_SOURCE_DIR}/vmtk-test-data
    DESTINATION ${VMTK_TESTING_DATA_INSTALL_LIB_DIR})


set(VMTK_TESTING_DIRECTORIES
  test_PypeS
  test_vmtkScripts
  )

foreach (TEST_DIR ${VMTK_TESTING_DIRECTORIES})
  add_subdirectory(${TEST_DIR})
endforeach ()