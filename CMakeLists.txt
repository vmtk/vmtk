CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT(VMTK)

# vmtk versions number
SET(VMTK_VERSION_MAJOR 1)
SET(VMTK_VERSION_MINOR 4)
SET(VMTK_VERSION_PATCH 0)
SET(VMTK_VERSION
    "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}.${VMTK_VERSION_PATCH}")

# Append the library version information to the library target properties.
OPTION(VMTK_WITH_LIBRARY_VERSION "Build with library version information" OFF)
IF(VMTK_WITH_LIBRARY_VERSION)
  # This setting of SOVERSION assumes that any API change
  # will increment either the minor or major version number of vmtk.
  SET(VMTK_LIBRARY_PROPERTIES
      VERSION "${VMTK_VERSION}"
      SOVERSION "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}"
     )
ENDIF(VMTK_WITH_LIBRARY_VERSION)

FIND_PACKAGE( PythonInterp )
IF (NOT VMTK_PYTHON_VERSION)
  SET(VMTK_PYTHON_VERSION "python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}" CACHE STRING "" FORCE)
ENDIF (NOT VMTK_PYTHON_VERSION)
MARK_AS_ADVANCED(VMTK_PYTHON_VERSION)

IF (APPLE AND VMTK_BREW_PYTHON)
  IF(VMTK_BREW_PYTHON)
    OPTION(VMTK_BREW_PYTHON "Link against Homebrew Python" ON)
    SET(PYTHON_SHEBANG ${PYTHON_EXECUTABLE})
  ELSE(VMTK_BREW_PYTHON)
    SET(PYTHON_SHEBANG "/usr/bin/env python")
  ENDIF(VMTK_BREW_PYTHON)
  #IF(VMTK_BREW_PYTHON)
  #  EXECUTE_PROCESS(
  #    COMMAND /usr/local/bin/python -c "from distutils import sysconfig; print sysconfig.get_python_inc()"
  #    OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
  #    OUTPUT_STRIP_TRAILING_WHITESPACE)
  #  SET(PYTHON_LIBRARY ${PYTHON_INCLUDE_DIR}../../lib/lib${VMTK_PYTHON_VERSION}.dylib)
  #ENDIF(VMTK_BREW_PYTHON)
ELSEIF (UNIX)
  SET(PYTHON_SHEBANG "/usr/bin/env python")
ENDIF ()

option( VMTK_USE_X
  "Compile vmtk with X11." OFF )
if( VMTK_USE_X )
  set ( VTK_USE_X ON CACHE BOOL "" FORCE )
else ( VMTK_USE_X )
  set ( VTK_USE_X OFF CACHE BOOL "" FORCE )
endif( VMTK_USE_X )

#-----------------------------------------------------------------------------
# ITK Setup
option( USE_SYSTEM_ITK
  "Exclude ITK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_ITK )
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
endif( USE_SYSTEM_ITK )

SET(VMTK_RENDERING_BACKEND "OpenGL2" CACHE STRING "Graphic backend for VTK")

#-----------------------------------------------------------------------------
# VTK Setup
option( USE_SYSTEM_VTK
  "Exclude VTK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_VTK )
  find_package( VTK REQUIRED )
  if( ${VTK_VERSION_MAJOR} LESS 6 )
    include( ${VTK_USE_FILE} )
  endif( ${VTK_VERSION_MAJOR} LESS 6 )
endif( USE_SYSTEM_VTK )

#-----------------------------------------------------------------------------
# Superbuild setup
option( VMTK_USE_SUPERBUILD
  "Build VMTK and the projects it depends on via SuperBuild.cmake." ON )

# option( VMTK_USE_VTK7
#  "Compile vmtk against VTK 7. OFF is VTK 6.3." ON )

option( VMTK_USE_VTK8
  "Compile vmtk against VTK 8. OFF is VTK 7.1" ON )

option( VMTK_BUILD_TESTING
  "Build the VMTK testing libraries" OFF)

IF(VMTK_BUILD_TESTING)
  subdirs(tests)
ENDIF(VMTK_BUILD_TESTING)

option( VMTK_BUNJEE_BUILD
  "Build VMTK and the projects it depends on ready for being linked from Bunjee." OFF )

if( VMTK_USE_SUPERBUILD )

  if( NOT GIT_EXECUTABLE )
    find_package( Git REQUIRED )
  endif( NOT GIT_EXECUTABLE )

  option( GIT_PROTOCOL_HTTPS
    "Use HTTPS for git access (useful if behind a firewall)" OFF )
  if( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "https" CACHE STRING "Https protocol for file transfer" FORCE )
  else( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "git" CACHE STRING "Git protocol for file transfer" FORCE )
  endif( GIT_PROTOCOL_HTTPS )
  mark_as_advanced( GIT_PROTOCOL )

  if( VMTK_BUNJEE_BUILD )
    set ( VTK_VMTK_WRAP_PYTHON OFF CACHE BOOL "" FORCE )
    set ( VMTK_CONTRIB_SCRIPTS OFF CACHE BOOL "" FORCE )
    # set ( VMTK_ENABLE_DISTRIBUTION OFF CACHE BOOL "" FORCE )
    #set ( VMTK_BUILD_TETGEN OFF CACHE BOOL "" FORCE )
    set ( VMTK_SCRIPTS_ENABLED OFF CACHE BOOL "" FORCE )
    set ( BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
    set ( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
    # set ( BUILD_TESTING OFF CACHE BOOL "" FORCE )
    set ( VTK_USE_CARBON OFF )
    set ( VTK_USE_COCOA ON )
    set ( VTK_VMTK_USE_COCOA ON )
    set ( VTK_USE_INFOVIS OFF )
    set ( VTK_USE_N_WAY_ARRAYS OFF )
    set ( VTK_USE_PARALLEL OFF )
    set ( VTK_USE_QT OFF )
    set ( VMTK_USE_RENDERING ON )
    set ( VTK_USE_TEXT_ANALYSIS OFF )
    set (  OFF )
    set ( VTK_WRAP_PYTHON OFF )
    set ( VTK_WRAP_TCL OFF )
    set ( ITK_USE_FLAT_DIRECTORY_INSTALL ON )
    set ( CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE )
    set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
    set ( ITK_NO_LIBRARY_VERSION ON )
    set ( VTK_NO_LIBRARY_VERSION ON )
    set ( VMTK_WITH_LIBRARY_VERSION OFF )
    set ( VTK_REQUIRED_OBJCXX_FLAGS "-fobjc-gc" CACHE STRING "" FORCE)
  else (VMTK_BUNJEE_BUILD)
    option ( VTK_VMTK_WRAP_PYTHON "Generate Python wrappings for C++ classes." ON )
    if (UNIX)
      set ( VTK_USE_X ON )
    endif (UNIX)
    if (APPLE)
      set ( VTK_USE_X OFF )
    endif(APPLE)
    option ( VMTK_CONTRIB_SCRIPTS "Install modules from the vmtkScripts/contrib directory." ON )
    option ( VTK_VMTK_CONTRIB "Build and install classes in the vtkVmtk/Contrib directory." ON )
    # option ( VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF )
    option ( VMTK_BUILD_TETGEN "Build TetGen and TetGen wrapper. Check TetGen license before you activate this." ON )
    option ( VMTK_BUILD_STREAMTRACER "Build static temporal stream tracer." ON )
    if (APPLE)
      option ( VTK_VMTK_USE_COCOA "Build with Cocoa support." ON )
      set ( CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE )
    endif (APPLE)
    set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
    option ( BUILD_SHARED_LIBS "Build shared libraries." ON )
    option ( VMTK_USE_RENDERING "Build VMTK rendering classes." ON )
    #option ( VTK_USE_HYBRID "Build VTK Hybrid classes." ON )
    OPTION(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
    OPTION(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)
  endif (VMTK_BUNJEE_BUILD)

  include( "${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake" )
  return()

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

OPTION(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
MARK_AS_ADVANCED(VMTK_SCRIPTS_ENABLED)

IF(VMTK_SCRIPTS_ENABLED)
  SUBDIRS(
    PypeS
    vtkVmtk
    vmtkScripts
    )
ELSE(VMTK_SCRIPTS_ENABLED)
  SUBDIRS(
    vtkVmtk
    )
ENDIF(VMTK_SCRIPTS_ENABLED)

OPTION(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)

SET(LIBRARY_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(VMTK_SRCS
   vmtk.py
   )

SET(VMTK_MODULE_SRCS
   __init__.py.in
   )

IF(NOT VMTK_INSTALL_BIN_DIR)
  SET(VMTK_INSTALL_BIN_DIR bin)
ENDIF(NOT VMTK_INSTALL_BIN_DIR)

IF(NOT VMTK_INSTALL_LIB_DIR)
  SET(VMTK_INSTALL_LIB_DIR lib)
ENDIF(NOT VMTK_INSTALL_LIB_DIR)

IF(NOT VMTK_MODULE_INSTALL_LIB_DIR)
  SET(VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_INSTALL_LIB_DIR}/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
ENDIF(NOT VMTK_MODULE_INSTALL_LIB_DIR)

IF(VMTK_SCRIPTS_ENABLED)

  FOREACH (SCRIPT_FILE ${VMTK_SRCS})
    CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
  ENDFOREACH (SCRIPT_FILE)

  CONFIGURE_FILE(${VMTK_SOURCE_DIR}/__init__.py.in ${VMTK_BINARY_DIR}/__init__.py COPYONLY)

  INSTALL(FILES __init__.py.in
    DESTINATION ${VMTK_MODULE_INSTALL_LIB_DIR}
    COMPONENT PythonRuntimeLibraries
    RENAME __init__.py)

  IF (NOT WIN32)

    SET (STRIPPED_SCRIPTS_SRCS )

    FOREACH (SCRIPT_FILE ${VMTK_SRCS})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      STRING(REGEX REPLACE ".py" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
      SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    ENDFOREACH (SCRIPT_FILE)

    INSTALL(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  ELSE (NOT WIN32)

    FOREACH (SCRIPT_FILE ${VMTK_SRCS})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      STRING(REGEX REPLACE ".py" "-exe.py" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
      SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    ENDFOREACH (SCRIPT_FILE)

    INSTALL(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  ENDIF (NOT WIN32)

  # OPTION(VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF)
  # IF(VMTK_ENABLE_DISTRIBUTION)
  #   ADD_SUBDIRECTORY(distribution)
  # ENDIF(VMTK_ENABLE_DISTRIBUTION)

  INSTALL(EXPORT VMTK-Targets DESTINATION ${VMTK_BINARY_DIR})

  CONFIGURE_FILE(
    ${VMTK_SOURCE_DIR}/CMake/VMTKConfig.cmake.in
    ${VMTK_BINARY_DIR}/VMTKConfig.cmake
  @ONLY IMMEDIATE
  )

  CONFIGURE_FILE(
    ${VMTK_SOURCE_DIR}/CMake/VMTKUse.cmake.in
    ${VMTK_BINARY_DIR}/VMTKUse.cmake
  @ONLY IMMEDIATE
  )

ENDIF(VMTK_SCRIPTS_ENABLED)
