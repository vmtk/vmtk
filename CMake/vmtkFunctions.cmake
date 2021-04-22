include(CMakeParseArguments)

set(VMTK_VTK_WRAP_HIERARCHY_DIR ${VMTK_BINARY_DIR})
set(VMTK_VTK_WRAP_MODULE_INSTALL_COMPONENT_IDENTIFIER "PythonRuntimeLibraries")
set(VMTK_VTK_WRAP_HIERARCHY_TARGETS_PROPERTY_NAME "VMTK_WRAP_HIERARCHY_TARGETS")
include(${VMTK_SOURCE_DIR}/CMake/vtkMacroKitPythonWrap.cmake)

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

  set(lib_name ${VMTK_LIB_NAME})

  if(NOT VMTK_LIB_NAME MATCHES "^vtkvmtk")
    set(lib_name vtkvmtk${VMTK_LIB_NAME})
  endif()

  add_library(${lib_name} ${VMTK_LIB_SRCS})

  if(VMTK_LIBRARY_PROPERTIES)
    set_target_properties(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
  endif(VMTK_LIBRARY_PROPERTIES)

  set_target_properties(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
  if (APPLE)
    set_target_properties( ${lib_name} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" )
  endif (APPLE)

  if(NOT WIN32)
    set_target_properties(${lib_name} PROPERTIES COMPILE_FLAGS -fPIC)
  endif(NOT WIN32)

  if(VMTK_LIB_TARGET_LINK_LIBRARIES)
    if (APPLE)
      set(vmtk_target_link_libs )
      foreach(lib IN LISTS VMTK_LIB_TARGET_LINK_LIBRARIES)
        if(NOT lib MATCHES "python")
          list(APPEND vmtk_target_link_libs ${lib})
        endif()
      endforeach()
      target_link_libraries(${lib_name} ${vmtk_target_link_libs})
    else (APPLE)
      target_link_libraries(${lib_name} ${VMTK_LIB_TARGET_LINK_LIBRARIES})
    endif (APPLE)
  endif()

  install(TARGETS ${lib_name}
    EXPORT VMTK-Targets
    LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
    RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
  )

  file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  install(FILES ${files}
    DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)
  install(FILES ${VTK_VMTK_SEGMENTATION_ITK_HEADERS}
    DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)

  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

    set(vmtk_wrapped_libs )
    foreach(lib IN LISTS VMTK_LIB_TARGET_LINK_LIBRARIES)
      if(lib MATCHES "^vtkvmtk" AND TARGET ${lib}PythonD)
        list(APPEND vmtk_wrapped_libs ${lib})
      endif()
    endforeach()

    if(NOT DEFINED VTK_VMTK_WRAPPED_MODULE_INSTALL_LIB_DIR)
      set(VTK_VMTK_WRAPPED_MODULE_INSTALL_LIB_DIR ${VTK_VMTK_MODULE_INSTALL_LIB_DIR})
    endif()

    vtkMacroKitPythonWrap(
      KIT_NAME ${lib_name}
      KIT_SRCS ${VMTK_LIB_SRCS}
      KIT_PYTHON_LIBRARIES ${vmtk_wrapped_libs}
      KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
      KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
      KIT_MODULE_INSTALL_LIB_DIR ${VTK_VMTK_WRAPPED_MODULE_INSTALL_LIB_DIR}
      )
  endif (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)

endfunction()
