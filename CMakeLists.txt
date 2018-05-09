cmake_minimum_required(VERSION 3.3)
set ( CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE )

project(VMTK)

# vmtk versions number
set(VMTK_VERSION_MAJOR 1)
set(VMTK_VERSION_MINOR 4)
set(VMTK_VERSION_PATCH 0)
set(VMTK_VERSION
    "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}.${VMTK_VERSION_PATCH}")

# Append the library version information to the library target properties.
option(VMTK_WITH_LIBRARY_VERSION "Build with library version information" OFF)
if(VMTK_WITH_LIBRARY_VERSION)
  # This setting of SOVERSION assumes that any API change
  # will increment either the minor or major version number of vmtk.
  set(VMTK_LIBRARY_PROPERTIES
      VERSION "${VMTK_VERSION}"
      SOVERSION "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}"
     )
endif(VMTK_WITH_LIBRARY_VERSION)

# Python Settings
find_package( PythonInterp REQUIRED )
find_package( PythonLibs REQUIRED )

if (NOT VMTK_PYTHON_VERSION)
  set(VMTK_PYTHON_VERSION "python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}" CACHE STRING "" FORCE)
endif (NOT VMTK_PYTHON_VERSION)
mark_as_advanced(VMTK_PYTHON_VERSION)

set(PYTHON_SHEBANG "/usr/bin/env python")

# General Settings
set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
set ( BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)


option( VMTK_USE_X "Compile vmtk with X11." OFF )
if( VMTK_USE_X )
  set ( VTK_USE_X ON CACHE BOOL "" FORCE )
else ( VMTK_USE_X )
  set ( VTK_USE_X OFF CACHE BOOL "" FORCE )
endif( VMTK_USE_X )

#-----------------------------------------------------------------------------
# ITK setup
option( USE_SYSTEM_ITK
  "Exclude ITK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_ITK )
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
endif( USE_SYSTEM_ITK )

set(VMTK_RENDERING_BACKEND "OpenGL2" CACHE STRING "Graphic backend for VTK")

#-----------------------------------------------------------------------------
# VTK setup
option( USE_SYSTEM_VTK "Exclude VTK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_VTK )
  find_package( VTK REQUIRED )
  if( ${VTK_VERSION_MAJOR} LESS 6 )
    include( ${VTK_USE_FILE} )
  endif( ${VTK_VERSION_MAJOR} LESS 6 )
endif( USE_SYSTEM_VTK )

option( VMTK_USE_VTK8 "Compile vmtk against VTK 8. OFF is VTK 7.1" ON )


#---------------------------------------------------------------------------
# Testing Setup
option( VMTK_BUILD_TESTING "Build the VMTK testing libraries" OFF)
if( VMTK_BUILD_TESTING )
  subdirs( tests )
endif( VMTK_BUILD_TESTING )


#-----------------------------------------------------------------------------
# Superbuild setup
option( VMTK_USE_SUPERBUILD "Build VMTK and the projects it depends on via SuperBuild.cmake." ON )

##
## Superbuild ON
##
if( VMTK_USE_SUPERBUILD )

  if( NOT GIT_EXECUTABLE )
    find_package( Git REQUIRED )
  endif( NOT GIT_EXECUTABLE )
  option( GIT_PROTOCOL_HTTPS "Use HTTPS for git access (useful if behind a firewall)" OFF )
  if( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "https" CACHE STRING "Https protocol for file transfer" FORCE )
  else( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "git" CACHE STRING "Git protocol for file transfer" FORCE )
  endif( GIT_PROTOCOL_HTTPS )
  mark_as_advanced( GIT_PROTOCOL )

  if (UNIX)
    set ( VTK_USE_X ON )
  endif (UNIX)
  if(APPLE)
    set ( VTK_USE_X OFF )
    option ( VTK_VMTK_USE_COCOA "Build with Cocoa support." ON )
  endif(APPLE)

  option ( VTK_VMTK_WRAP_PYTHON "Generate Python wrappings for C++ classes." ON )
  option ( VMTK_CONTRIB_SCRIPTS "Install modules from the vmtkScripts/contrib directory." ON )
  option ( VTK_VMTK_CONTRIB "Build and install classes in the vtkVmtk/Contrib directory." ON )
  option ( VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF )
  option ( VMTK_BUILD_TETGEN "Build TetGen and TetGen wrapper. Check TetGen license before you activate this." ON )
  option ( VMTK_BUILD_STREAMTRACER "Build static temporal stream tracer." ON )
  option ( VMTK_USE_RENDERING "Build VMTK rendering classes." ON )
  option ( VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
  option ( VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)

  include( "${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake" )
  return()
##
## Superbuild OFF
##
else( VMTK_USE_SUPERBUILD )

    if ( NOT ITK_FOUND )
      find_package( ITK REQUIRED )
      include( ${ITK_USE_FILE} )
    endif( NOT ITK_FOUND )

    if ( NOT VTK_FOUND )
      find_package( VTK REQUIRED )
      if( ${VTK_VERSION_MAJOR} LESS 6 )
        include( ${VTK_USE_FILE} )
      endif( ${VTK_VERSION_MAJOR} LESS 6 )
    endif( NOT VTK_FOUND )

endif( VMTK_USE_SUPERBUILD )
#
#------------------------------------------------------------------------------------

option(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
option(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)
mark_as_advanced(VMTK_SCRIPTS_ENABLED VMTK_MINIMAL_INSTALL)

if(VMTK_SCRIPTS_ENABLED)
  subdirs(
    PypeS
    vtkVmtk
    vmtkScripts
    )
else(VMTK_SCRIPTS_ENABLED)
  subdirs(
    vtkVmtk
    )
endif(VMTK_SCRIPTS_ENABLED)

set(VMTK_SRCS
   vmtk.py
   )

set(LIBRARY_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

# Bin directory default (will be appended to superbuild prefix if using it)
if(NOT VMTK_INSTALL_BIN_DIR)
  set(VMTK_INSTALL_BIN_DIR bin)
endif(NOT VMTK_INSTALL_BIN_DIR)

# Lib directory default (will be appended to superbuild prefix if using it)
if(NOT VMTK_INSTALL_LIB_DIR)
  set(VMTK_INSTALL_LIB_DIR lib)
endif(NOT VMTK_INSTALL_LIB_DIR)

# VMTK Python Lib default directory (will be appended to superbuild prefix if using it)
# Note: PypeS and vmtkScripts Module have their own Install Lib Dirs.
# This is where the .so runtime libs will go
if(NOT VMTK_MODULE_INSTALL_LIB_DIR)
  set(VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_INSTALL_LIB_DIR}/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
endif(NOT VMTK_MODULE_INSTALL_LIB_DIR)

#------------------------------------------------------------------------------------
# vmtk_scripts setup
if(VMTK_SCRIPTS_ENABLED)

  foreach (SCRIPT_FILE ${VMTK_SRCS})
    configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
  endforeach (SCRIPT_FILE)

  install(FILES __init__.py vmtkinteractive.py
    DESTINATION ${VMTK_MODULE_INSTALL_LIB_DIR}
    COMPONENT PythonRuntimeLibraries
    )

  ##  
  ## Linux or MacOS
  ##
  if (NOT WIN32)

    set (STRIPPED_SCRIPTS_SRCS )

    foreach (SCRIPT_FILE ${VMTK_SRCS})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      string(REGEX REPLACE ".py" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
      set (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    endforeach (SCRIPT_FILE)

    install(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  ##
  ## Windows
  ##
  else (NOT WIN32)

    foreach (SCRIPT_FILE ${VMTK_SRCS})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      string(REGEX REPLACE ".py" "-exe.py" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
      set (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    endforeach (SCRIPT_FILE)

    install(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  endif (NOT WIN32)
  

  install(EXPORT VMTK-Targets DESTINATION ${VMTK_BINARY_DIR})

  configure_file(
    ${VMTK_SOURCE_DIR}/CMake/VMTKConfig.cmake.in
    ${VMTK_BINARY_DIR}/VMTKConfig.cmake
  @ONLY IMMEDIATE
  )

  configure_file(
    ${VMTK_SOURCE_DIR}/CMake/VMTKUse.cmake.in
    ${VMTK_BINARY_DIR}/VMTKUse.cmake
  @ONLY IMMEDIATE
  )

endif(VMTK_SCRIPTS_ENABLED)