#
# Build the documentation
#
include (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)
                                                                                                               
if (BUILD_DOCUMENTATION)
                                                                                                               
  #
  # Configure the script and the doxyfile, then add target
  #
  configure_file(
    ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/doxyfile.in
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doxyfile)
                                                                                                               
  configure_file(
    ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/doc_makeall.sh.in
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doc_makeall.sh)

  add_custom_target(DoxygenDoc
    ${BASH}
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doc_makeall.sh)
                                                                                                               
  include (${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/LocalUserOptions.cmake OPTIONAL)
  
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.css$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.gif$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.html$")
  install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.png$")
  #install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.pl$")
  #install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.stop$")
  #install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen .txt )
  #install_files(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen .txt authors doc_readme)

endif ()
                                                                                                               
