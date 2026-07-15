#
# Build the documentation
#
# Deprecated by CMake policy CMP0106: include (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

if (BUILD_DOCUMENTATION)

  find_package(Doxygen OPTIONAL_COMPONENTS dot)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is required to build the documentation (BUILD_DOCUMENTATION=ON). Install Doxygen or set BUILD_DOCUMENTATION=OFF.")
  endif ()

  if (TARGET Doxygen::dot)
    get_target_property(DOXYGEN_DOT_PATH Doxygen::dot IMPORTED_LOCATION)
    get_filename_component(DOXYGEN_DOT_PATH "${DOXYGEN_DOT_PATH}" DIRECTORY)
  else ()
    set(DOXYGEN_DOT_PATH "")
  endif ()

  #
  # Configure the doxyfile, then add target
  #
  configure_file(
    ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/doxyfile.in
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doxyfile)

  add_custom_target(DoxygenDoc
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doc"
    COMMAND Doxygen::doxygen "${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doxyfile"
    WORKING_DIRECTORY "${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen"
    COMMENT "Generating VMTK API documentation with Doxygen"
    VERBATIM)

  include (${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/LocalUserOptions.cmake OPTIONAL)

  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.css$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.gif$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.html$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.png$")

endif ()
