include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "vmtk")
set(CPACK_PACKAGE_VENDOR "vmtk.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vmtk - the Vascular Modeling Toolkit")
set(CPACK_RESOURCE_FILE_LICENSE "${VMTK_SOURCE_DIR}/Copyright.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${VMTK_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VMTK_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VMTK_VERSION_PATCH})

set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${VMTK_BINARY_DIR};VMTK;ALL;/")
if(EXISTS "${VTK_DIR}/CMakeCache.txt")
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${VTK_DIR};VTK;ALL;/")
endif(EXISTS "${VTK_DIR}/CMakeCache.txt")
if(EXISTS "${ITK_DIR}/CMakeCache.txt")
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${ITK_DIR};ITK;ALL;/")
endif(EXISTS "${ITK_DIR}/CMakeCache.txt")

set(CPACK_COMPONENTS_ALL Development RuntimeLibraries RuntimeExecutables)

set(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DISPLAY_NAME "Application files")
set(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DESCRIPTION
  "The vmtk application and required components.")
set(CPACK_COMPONENT_RUNTIMEEXECUTABLES_DEPENDS RuntimeLibraries)

set(CPACK_COMPONENT_RUNTIMELIBRARIES_DISPLAY_NAME "Runtime libraries")
set(CPACK_COMPONENT_RUNTIMELIBRARIES_DESCRIPTION
  "Runtime libraries and Python modules.")

set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION
  "Static libraries and header files needed to build applications against vmtk.")
set(CPACK_COMPONENT_DEVELOPMENT_DISABLED ON)

if(EXISTS "${VTK_DIR}/Wrapping/Python/vtk")
  install(DIRECTORY
    "${VTK_DIR}/Wrapping/Python/vtk"
    DESTINATION lib/site-packages
    USE_SOURCE_PERMISSIONS
    COMPONENT RuntimeLibraries
    )
endif(EXISTS "${VTK_DIR}/Wrapping/Python/vtk")

if(APPLE)
  # Use the bundle-generator on osx ...
  set(CPACK_GENERATOR "Bundle")
  set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/bundle/vmtk.icns")
  set(CPACK_BUNDLE_NAME "vmtk")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bundle/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/egg/setup.py.in ${CMAKE_CURRENT_BINARY_DIR}/egg/setup.py)
  #configure_file(${CMAKE_CURRENT_SOURCE_DIR}/homebrew/vmtk.rb.in ${CMAKE_CURRENT_BINARY_DIR}/homebrew/vmtk.rb)
  set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist")
  set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/bundle/vmtk-startup")
  ##set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/bundle/CustomVolumeIcon.icns")
  #if(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
    set(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-darwin-${CMAKE_OSX_ARCHITECTURES}")
  #else(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
  #  set(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-darwin-universal")
  #endif(${CMAKE_OSX_ARCHITECTURES} STREQUAL "i386" OR ${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
endif(APPLE)

if(WIN32)
  # Use the nsis-generator on Windows ...
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_HELP_LINK "http://www.vmtk.org/Main/Installation")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.vmtk.org")
  set(CPACK_NSIS_MENU_LINKS "http://www.vmtk.org/Main/Documentation" "vmtk documentation")
  set(CPACK_NSIS_CONTACT "vmtk-users@lists.sourceforge.net")
  #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  #set(CPACK_NSIS_MUI_ICON "installer.ico")
  set(CPACK_NSIS_MODIFY_PATH "OFF")
  set(CPACK_NSIS_CREATE_ICONS
    "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\PypePad.lnk' '\$INSTDIR\\\\vmtk-startup.bat' '' '' '' SW_SHOWMINIMIZED")
  set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\PypePad.lnk'")

  install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/nsis/vmtk-startup.bat"
    DESTINATION .
    COMPONENT RuntimeExecutables
    )

  # Find and install Python
  option(VMTK_INSTALL_PYTHON "Include Python in the vmtk installer" OFF)
  if(VTK_VMTK_WRAP_PYTHON AND VMTK_INSTALL_PYTHON)
    find_program(VMTK_PYTHON_COMMAND python.exe
      PATHS "C:/Python26"
      DOC "Path to the installed python.exe."
      )
    if(EXISTS ${PYTHON_DIR})
      set(VMTK_PYTHON_COMMAND "${PYTHON_DIR}/python.exe")
    endif(EXISTS ${PYTHON_DIR})
  
    if(NOT VMTK_PYTHON_COMMAND)
      message(SEND_ERROR "Could not locate python.exe.")
    endif(NOT VMTK_PYTHON_COMMAND)

    get_filename_component(VMTK_PYTHON_PATH ${VMTK_PYTHON_COMMAND} PATH)
    install(DIRECTORY
      ${VMTK_PYTHON_PATH}/
      DESTINATION lib/Python
      USE_SOURCE_PERMISSIONS
      COMPONENT RuntimeExecutables
      )
  endif(VTK_VMTK_WRAP_PYTHON AND VMTK_INSTALL_PYTHON)

  if(MINGW)
    set(CPACK_PACKAGE_FILE_NAME "vmtk-${VMTK_VERSION}-mingw32")

    # Install mingwm10.dll
    find_program(VMTK_MINGW_GCC mingw32-gcc.exe
      PATHS ${MINGW_DIR} "C:/mingw/bin"
      DOC "Path to the MinGW GCC command."
      )
  
    get_filename_component(VMTK_MINGW_PATH ${VMTK_MINGW_GCC} PATH)
    install(FILES 
      ${VMTK_MINGW_PATH}/mingwm10.dll
      DESTINATION bin
      COMPONENT RuntimeExecutables
      )

    install(FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/nsis/mingw-postinst.bat
      DESTINATION .
      COMPONENT RuntimeExecutables
      )
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
      "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
      ExecWait '\\\"$INSTDIR\\\\mingw-postinst.bat\\\"'
      Delete \\\"$INSTDIR\\\\mingw-postinst.bat\\\""
      )
  endif(MINGW)

  if(MSVC)

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
      if(${MSVC_VERSION})
        find_program(MSVC_REDIST NAMES
    vcredist_${CMAKE_MSVC_ARCH}/vcredist_${CMAKE_MSVC_ARCH}.exe
          PATHS
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\${VCVERS}.0;InstallDir]/../../SDK/v${SDKVERS}/BootStrapper/Packages/"
          )
        get_filename_component(vcredist_name "${MSVC_REDIST}" NAME)
        install(PROGRAMS ${MSVC_REDIST} COMPONENT SupportFiles DESTINATION bin)
        set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait
    '\\\"$INSTDIR\\\\bin\\\\${vcredist_name}\\\"'")
        message(STATUS "MSVC_REDIST: ${MSVC_REDIST}")
      endif(${MSVC_VERSION})
    #endmacro()

    #if(${CMAKE_MSVC_ARCH} STREQUAL "amd64")
    #  set(VCREDIST_EXECUTABLE "vcredist_x64.exe")
    #else(${CMAKE_MSVC_ARCH} STREQUAL "amd64")
    #  set(VCREDIST_EXECUTABLE "vcredist_x86.exe")
    #endif(${CMAKE_MSVC_ARCH} STREQUAL "amd64")

    #find_program(VCREDIST_EXE
    #  ${VCREDIST_EXECUTABLE}
    #  PATHS 
    #  DOC "Path to the Visual Studio C++ redistributable"
    #  )

    #if(NOT VCREDIST_EXE)
    #  message(SEND_ERROR "Could not locate ${VCREDIST_EXECUTABLE}.")
    #endif(NOT VCREDIST_EXE)

    #install(FILES
    #  ${VCREDIST_EXE}
    #  DESTINATION redist
    #  COMPONENT RuntimeExecutables
    #  )

    #set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    #  "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    #  ExecWait '\\\"$INSTDIR\\\\redist\\\\${VCREDIST_EXECUTABLE}\\\"'")
  endif(MSVC)

  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/nsis/postinst.bat
    DESTINATION .
    COMPONENT RuntimeExecutables
    )
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    ExecWait '\\\"$INSTDIR\\\\postinst.bat\\\"'
    Delete \\\"$INSTDIR\\\\postinst.bat\\\""
    )

  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
    "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
    MessageBox MB_YESNO|MB_ICONQUESTION 'Do you want to completely remove the directory $INSTDIR and all of its contents?' IDNO NoDelete
      RMDir /r \\\"$INSTDIR\\\" ; skipped if no
    NoDelete:"
    )
endif(WIN32)

include(CPack)
