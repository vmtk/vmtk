
include(CMakeParseArguments)
include(${VTK_VMTK_SOURCE_DIR}/vtkMacroKitPythonWrap.cmake)

function(vmtk_build_library)
  set(options)
  set(oneValueArgs NAME)
  set(multiValueArgs SRCS TARGET_LINK_LIBRARIES INCLUDE_DIRECTORIES)
  cmake_parse_arguments(VMTK_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(expected_defined_vars NAME SRCS)
  foreach(var ${expected_defined_vars})
    if(NOT DEFINED VMTK_LIB_${var})
      message(FATAL_ERROR "error: ${var} parameter is mandatory !")
    endif()
  endforeach()

  if(VMTK_LIB_INCLUDE_DIRECTORIES)
    include_directories(${VMTK_LIB_INCLUDE_DIRECTORIES})
  endif()

  SET(lib_name ${VMTK_LIB_NAME})
  if(NOT VMTK_LIB_NAME MATCHES "^vtkvmtk")
    SET(lib_name vtkvmtk${VMTK_LIB_NAME})
  endif()

  ADD_LIBRARY(${lib_name} ${VMTK_LIB_SRCS})
  IF(VMTK_LIBRARY_PROPERTIES)
    SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
  ENDIF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
  IF(NOT WIN32)
    SET_TARGET_PROPERTIES(${lib_name} PROPERTIES COMPILE_FLAGS -fPIC)
  ENDIF(NOT WIN32)
  if(VMTK_LIB_TARGET_LINK_LIBRARIES)
    TARGET_LINK_LIBRARIES(${lib_name} ${VMTK_LIB_TARGET_LINK_LIBRARIES})
  endif()


  INSTALL(TARGETS ${lib_name}
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
  )

  FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  INSTALL(FILES ${files}
    DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)
  INSTALL(FILES ${VTK_VMTK_SEGMENTATION_ITK_HEADERS}
    DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)

  IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

    set(vmtk_wrapped_libs )
    foreach(lib IN LISTS VMTK_LIB_TARGET_LINK_LIBRARIES)
      if(lib MATCHES "^vtkvmtk" AND TARGET ${lib}PythonD)
        list(APPEND vmtk_wrapped_libs ${lib})
      endif()
    endforeach()

    vtkMacroKitPythonWrap(
      KIT_NAME ${lib_name}
      KIT_SRCS ${VMTK_LIB_SRCS}
      KIT_PYTHON_LIBRARIES ${vmtk_wrapped_libs}
      KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
      KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
      KIT_MODULE_INSTALL_LIB_DIR ${VTK_VMTK_MODULE_INSTALL_LIB_DIR}
      )
  ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

endfunction()
