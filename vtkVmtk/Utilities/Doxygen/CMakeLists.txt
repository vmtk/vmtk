#
# Build the documentation
#
INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)
                                                                                                               
IF (BUILD_DOCUMENTATION)
                                                                                                               
  #
  # Configure the script and the doxyfile, then add target
  #
  CONFIGURE_FILE(
    ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/doxyfile.in
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doxyfile)
                                                                                                               
  CONFIGURE_FILE(
    ${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/doc_makeall.sh.in
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doc_makeall.sh)

  ADD_CUSTOM_TARGET(DoxygenDoc
    ${BASH}
    ${VTK_VMTK_BINARY_DIR}/Utilities/Doxygen/doc_makeall.sh)
                                                                                                               
  INCLUDE (${VTK_VMTK_SOURCE_DIR}/Utilities/Doxygen/LocalUserOptions.cmake OPTIONAL)
  
  INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.css$")
  INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.gif$")
  INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.html$")
  INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.png$")
  #INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.pl$")
  #INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen "\\.stop$")
  #INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen .txt )
  #INSTALL_FILES(${VTK_VMTK_INSTALL_LIB_DIR}/doxygen .txt authors doc_readme)

ENDIF (BUILD_DOCUMENTATION)
                                                                                                               
