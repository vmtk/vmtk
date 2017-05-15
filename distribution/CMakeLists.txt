INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_NAME "vmtk")
SET(CPACK_PACKAGE_VENDOR "vmtk.org")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vmtk - the Vascular Modeling Toolkit")
SET(CPACK_RESOURCE_FILE_LICENSE "${VMTK_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VMTK_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VMTK_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VMTK_VERSION_PATCH})

SET(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${VMTK_BINARY_DIR};VMTK;ALL;/")
IF(EXISTS "${VTK_DIR}/CMakeCache.txt")
  SET(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${VTK_DIR};VTK;ALL;/")
ENDIF(EXISTS "${VTK_DIR}/CMakeCache.txt")
IF(EXISTS "${ITK_DIR}/CMakeCache.txt")
  SET(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${ITK_DIR};ITK;ALL;/")
ENDIF(EXISTS "${ITK_DIR}/CMakeCache.txt")

SET(CPACK_COMPONENTS_ALL Development RuntimeLibraries RuntimeExecutables)

SET(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DISPLAY_NAME "Application files")
SET(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DESCRIPTION
  "The vmtk application and required components.")
set(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DEPENDS RuntimeLibraries)

SET(CPACK_COMPONENT_RUNTIMELIBRARIES_DISPLAY_NAME "Runtime libraries")
SET(CPACK_COMPONENT_RUNTIMELIBRARIES_DESCRIPTION
  "Runtime libraries and Python modules.")

SET(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development files")
SET(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION
  "Static libraries and header files needed to build applications against vmtk.")
SET(CPACK_COMPONENT_DEVELOPMENT_DISABLED ON)

IF(EXISTS "${VTK_DIR}/Wrapping/Python/vtk")
  INSTALL(DIRECTORY
    "${VTK_DIR}/Wrapping/Python/vtk"
    DESTINATION lib/site-packages
    USE_SOURCE_PERMISSIONS
    COMPONENT RuntimeLibraries
    )
ENDIF(EXISTS "${VTK_DIR}/Wrapping/Python/vtk")

IF(APPLE)
  # Use the bundle-generator on osx ...
  SET(CPACK_GENERATOR "Bundle")
  SET(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/bundle/vmtk.icns")
  SET(CPACK_BUNDLE_NAME "vmtk")
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/bundle/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist @ONLY)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/egg/setup.py.in ${CMAKE_CURRENT_BINARY_DIR}/egg/setup.py)
  #CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/homebrew/vmtk.rb.in ${CMAKE_CURRENT_BINARY_DIR}/homebrew/vmtk.rb)
  SET(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist")
  SET(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/bundle/vmtk-startup")
  ##SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/bundle/CustomVolumeIcon.icns")
  #IF(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
    SET(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-darwin-${CMAKE_OSX_ARCHITECTURES}")
  #ELSE(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
  #  SET(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-darwin-universal")
  #ENDIF(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
ENDIF(APPLE)

IF(WIN32)
  # Use the nsis-generator on Windows ...
  SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_NSIS_HELP_LINK "http://www.vmtk.org/Main/Installation")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.vmtk.org")
  SET(CPACK_NSIS_MENU_LINKS "http://www.vmtk.org/Main/Documentation" "vmtk documentation")
  SET(CPACK_NSIS_CONTACT "vmtk-users@lists.sourceforge.net")
  #SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  #SET(CPACK_NSIS_MUI_ICON "installer.ico")
  SET(CPACK_NSIS_MODIFY_PATH "OFF")
  SET(CPACK_NSIS_CREATE_ICONS
    "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\PypePad.lnk' '\$INSTDIR\\\\vmtk-startup.bat' '' '' '' SW_SHOWMINIMIZED")
  SET(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\PypePad.lnk'")

  INSTALL(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/nsis/vmtk-startup.bat"
    DESTINATION .
    COMPONENT RuntimeExecutables
    )

  # Find and install Python
  OPTION(VMTK_INSTALL_PYTHON "Include Python in the vmtk installer" OFF)
  IF(VTK_VMTK_WRAP_PYTHON AND VMTK_INSTALL_PYTHON)
    FIND_PROGRAM(VMTK_PYTHON_COMMAND python.exe
      PATHS "C:/Python26"
      DOC "Path to the installed python.exe."
      )
    IF(EXISTS ${PYTHON_DIR})
      SET(VMTK_PYTHON_COMMAND "${PYTHON_DIR}/python.exe")
    ENDIF(EXISTS ${PYTHON_DIR})
  
    IF(NOT VMTK_PYTHON_COMMAND)
      MESSAGE(SEND_ERROR "Could not locate python.exe.")
    ENDIF(NOT VMTK_PYTHON_COMMAND)

    GET_FILENAME_COMPONENT(VMTK_PYTHON_PATH ${VMTK_PYTHON_COMMAND} PATH)
    INSTALL(DIRECTORY
      ${VMTK_PYTHON_PATH}/
      DESTINATION lib/Python
      USE_SOURCE_PERMISSIONS
      COMPONENT RuntimeExecutables
      )
  ENDIF(VTK_VMTK_WRAP_PYTHON AND VMTK_INSTALL_PYTHON)

  IF(MINGW)
    SET(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-mingw32")

    # Install mingwm10.dll
    FIND_PROGRAM(VMTK_MINGW_GCC mingw32-gcc.exe
      PATHS ${MINGW_DIR} "C:/mingw/bin"
      DOC "Path to the MinGW GCC command."
      )
  
    GET_FILENAME_COMPONENT(VMTK_MINGW_PATH ${VMTK_MINGW_GCC} PATH)
    INSTALL(FILES 
      ${VMTK_MINGW_PATH}/mingwm10.dll
      DESTINATION bin
      COMPONENT RuntimeExecutables
      )

    INSTALL(FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/nsis/mingw-postinst.bat
      DESTINATION .
      COMPONENT RuntimeExecutables
      )
    SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
      "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
      ExecWait '\\\"$INSTDIR\\\\mingw-postinst.bat\\\"'
      Delete \\\"$INSTDIR\\\\mingw-postinst.bat\\\""
      )
  ENDIF(MINGW)

  IF(MSVC)

    #macro(_FIND_MSVC_REDIST)
      message(STATUS "Looking for MSVC Redistributable Executable for MSVC Version ${MSVC_VERSION}")
      set(SDKVERS "2.0")
      if(${MSVC_VERSION} EQUAL 1400)
        set(SDKVERS "2.0")
        set(VCVERS "8")
      endif()
      if(${MSVC_VERSION} EQUAL 1500)
        set(SDKVERS "3.5")
        set(VCVERS "9")
      endif()
      if(${MSVC_VERSION} EQUAL 1600)
        set(SDKVERS "3.5")
        set(VCVERS "10")
      endif()
      IF(${MSVC_VERSION})
        FIND_PROGRAM(MSVC_REDIST NAMES
    vcredist_${CMAKE_MSVC_ARCH}/vcredist_${CMAKE_MSVC_ARCH}.exe
          PATHS
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          )
        GET_FILENAME_COMPONENT(vcredist_name "${MSVC_REDIST}" NAME)
        INSTALL(PROGRAMS ${MSVC_REDIST} COMPONENT SupportFiles DESTINATION bin)
        SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait
    '\\\"$INSTDIR\\\\bin\\\\${vcredist_name}\\\"'")
        message(STATUS "MSVC_REDIST: ${MSVC_REDIST}")
      ENDIF(${MSVC_VERSION})
    #endmacro()

    #IF(${CMAKE_MSVC_ARCH} STREQUAL "amd64")
    #  SET(VCREDIST_EXECUTABLE "vcredist_x64.exe")
    #ELSE(${CMAKE_MSVC_ARCH} STREQUAL "amd64")
    #  SET(VCREDIST_EXECUTABLE "vcredist_x86.exe")
    #ENDIF(${CMAKE_MSVC_ARCH} STREQUAL "amd64")

    #FIND_PROGRAM(VCREDIST_EXE
    #  ${VCREDIST_EXECUTABLE}
    #  PATHS 
    #  DOC "Path to the Visual Studio C++ redistributable"
    #  )

    #IF(NOT VCREDIST_EXE)
    #  MESSAGE(SEND_ERROR "Could not locate ${VCREDIST_EXECUTABLE}.")
    #ENDIF(NOT VCREDIST_EXE)

    #INSTALL(FILES
    #  ${VCREDIST_EXE}
    #  DESTINATION redist
    #  COMPONENT RuntimeExecutables
    #  )

    #SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    #  "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    #  ExecWait '\\\"$INSTDIR\\\\redist\\\\${VCREDIST_EXECUTABLE}\\\"'")
  ENDIF(MSVC)

  INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/nsis/postinst.bat
    DESTINATION .
    COMPONENT RuntimeExecutables
    )
  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    ExecWait '\\\"$INSTDIR\\\\postinst.bat\\\"'
    Delete \\\"$INSTDIR\\\\postinst.bat\\\""
    )

  SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
    "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
    MessageBox MB_YESNO|MB_ICONQUESTION 'Do you want to completely remove the directory $INSTDIR and all of its contents?' IDNO NoDelete
      RMDir /r \\\"$INSTDIR\\\" ; skipped if no
    NoDelete:"
    )
ENDIF(WIN32)

INCLUDE(CPack)
