##############################################################################
#
# Library:   VMTK
#
##############################################################################
include( ExternalProject )

set(SUPERBUILD_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Install CACHE PATH
    "Path where all targets will be installed.")

set( base "${CMAKE_BINARY_DIR}" )
set_property( DIRECTORY PROPERTY EP_BASE ${base} )

#set( shared ON )
#if ( BUILD_SHARED_LIBS )
#  set( shared "${BUILD_SHARED_LIBS}" )
#endif()
#set( testing OFF )
#set( examples OFF )
#set( build_type "Debug" )
#if( CMAKE_BUILD_TYPE )
#  set( build_type "${CMAKE_BUILD_TYPE}" )
#endif()

set( VMTK_DEPENDS "" )

set( gen "${CMAKE_GENERATOR}" )

if( VTK_VMTK_WRAP_PYTHON )

  if (NOT Python3_FOUND)
    find_package(Python3 COMPONENTS Interpreter Development)
  endif()
  # if (APPLE AND VMTK_BREW_PYTHON)
  #   execute_process(
  #     COMMAND /usr/bin/env python-config --prefix
  #     OUTPUT_VARIABLE PYTHON_PREFIX
  #     OUTPUT_STRIP_TRAILING_WHITESPACE)
  #   set(PYTHON_INCLUDE_DIR ${PYTHON_PREFIX}/Headers CACHE PATH "")
  #   set(PYTHON_LIBRARY ${PYTHON_PREFIX}/Python CACHE FILEPATH "")
  # else ()
  #   find_package( PythonLibs )
  # endif ()

  if (WIN32)
    set( PYTHON_MAJORMINOR ${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} )
    string( REGEX MATCH "[0-9][0-9]" PYTHON_MIN_MINOR ${Python3_EXECUTABLE}  )
    string( REGEX MATCH "(.*[/])*" PYTHON_ROOT_DIR ${Python3_EXECUTABLE}  )
    file( TO_NATIVE_PATH ${PYTHON_ROOT_DIR} PYTHON_ROOT_DIR_NATIVE )
  endif ()

endif()


##
## Check if sytem ITK or superbuild ITK
##
if( NOT USE_SYSTEM_ITK )


  set(ITK_GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/Kitware/ITK.git")
  if (VMTK_USE_ITK5)
    set(ITK_GIT_TAG "v5.2.1")
    set( ITK_VERSION 5.2 )
  else ()
    set(ITK_GIT_TAG "v4.13.3")
    set( ITK_VERSION 4.13 )
  endif ()
  ##
  ## ITK
  ##
  set( proj ITK )
  ExternalProject_Add( ${proj}
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/Kitware/ITK.git"
    GIT_TAG ${ITK_GIT_TAG}
    SOURCE_DIR "${CMAKE_BINARY_DIR}/ITK"
    BINARY_DIR ITK-Build
    CMAKE_GENERATOR ${gen}
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_INSTALL 1
    CMAKE_ARGS
      -Dgit_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
      -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
      #-DCMAKE_CXX_FLAGS:STRING="-fPIC" #${CMAKE_CXX_FLAGS}
      #-DCMAKE_C_FLAGS:STRING="-fPIC" #${CMAKE_C_FLAGS}
      -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
      -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX:PATH=${SUPERBUILD_INSTALL_PREFIX}
      -DITK_USE_FLAT_DIRECTORY_INSTALL:BOOL=${ITK_USE_FLAT_DIRECTORY_INSTALL}
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      #-DBUILD_SHARED_LIBS:BOOL=OFF
      -DCMAKE_RC_COMPILER:BOOL=${CMAKE_RC_COMPILER}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DITK_USE_REVIEW:BOOL=ON
      -DITK_USE_REVIEW_STATISTICS:BOOL=ON
      -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON
      -DITK_LEGACY_REMOVE:BOOL=OFF   #<-- Allow LEGACY ITKv4 features for now.
      -DITK_LEGACY_SILENT:BOOL=OFF   #<-- Use of legacy code will produce compiler warnings
      -DModule_ITKDeprecated:BOOL=ON #<-- Needed for ITKv5 now. (itkMultiThreader.h and MutexLock backwards compatibility.)
      -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF
      -DCMAKE_MACOSX_RPATH=ON
      -DCMAKE_INSTALL_RPATH=${SUPERBUILD_INSTALL_PREFIX}/lib;@rpath
      -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
    INSTALL_DIR "${SUPERBUILD_INSTALL_PREFIX}/ITK"
    )
  set( ITK_DIR "${base}/ITK-Build" )

  set( VMTK_DEPENDS ${VMTK_DEPENDS} "ITK" )

endif()

##
## Check if sytem VTK or superbuild VTK
##
if( NOT USE_SYSTEM_VTK )

  set(VTK_GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/Kitware/VTK.git")
  if (VMTK_USE_VTK9)
    set(VTK_GIT_TAG "v9.0.3")
    set( VTK_VERSION 9.0 )
  else ()
    set(VTK_GIT_TAG "v8.1.2")
    set( VTK_VERSION 8.0 )
  endif ()


  ##
  ## VTK
  ##
  set( proj VTK )
  ExternalProject_Add( VTK
    GIT_REPOSITORY ${VTK_GIT_REPOSITORY}
    GIT_TAG ${VTK_GIT_TAG}
    SOURCE_DIR "${CMAKE_BINARY_DIR}/VTK"
    BINARY_DIR VTK-Build
    CMAKE_GENERATOR ${gen}
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_INSTALL 1
    CMAKE_ARGS
      -Dgit_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
      -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
      -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX:PATH=${SUPERBUILD_INSTALL_PREFIX}
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      -DVTK_WRAP_PYTHON:BOOL=${VTK_VMTK_WRAP_PYTHON}
      -DVTK_USE_TK:BOOL=OFF
      -DVTK_USE_CARBON:BOOL=${VTK_USE_CARBON}
      -DVTK_USE_COCOA:BOOL=${VTK_VMTK_USE_COCOA}
      -DVTK_USE_INFOVIS:BOOL=${VTK_USE_INFOVIS}
      -DVTK_USE_N_WAY_ARRAYS:BOOL=${VTK_USE_N_WAY_ARRAYS}
      -DVTK_USE_PARALLEL:BOOL=${VTK_USE_PARALLEL}
      -DVTK_USE_QT:BOOL=${VTK_USE_QT}
      -DVTK_Group_Rendering:BOOL=${VMTK_USE_RENDERING}
      #-DVTK_USE_HYBRID:BOOL=${VTK_USE_HYBRID}
      -DVTK_USE_TEXT_ANALYSIS:BOOL=${VTK_USE_TEXT_ANALYSIS}
      -DVTK_USE_X:BOOL=${VTK_USE_X}
      -DVTK_PYTHON_VERSION=${Python3_VERSION_MAJOR}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DVTK_USE_GUISUPPORT:BOOL=OFF
      -DVTK_INSTALL_PYTHON_USING_CMAKE:BOOL=ON
      -DVTK_RENDERING_BACKEND:STRING=${VMTK_RENDERING_BACKEND}
      -DCMAKE_RC_COMPILER:BOOL=${CMAKE_RC_COMPILER}
      -DPython${Python3_VERSION_MAJOR}_ROOT_DIR=${Python3_ROOT_DIR}
      -DPython${Python3_VERSION_MAJOR}_DEBUG_LIBRARY=${Python3_DEBUG_LIBRARY}
      -DPython${Python3_VERSION_MAJOR}_EXECUTABLE=${Python3_EXECUTABLE}
      -DPython${Python3_VERSION_MAJOR}_INCLUDE_DIR=${Python3_DEBUG_LIBRARY}
      -DPython${Python3_VERSION_MAJOR}_LIBRARY=${Python3_LIBRARY}
      -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF
      -DCMAKE_MACOSX_RPATH=ON
      -DCMAKE_INSTALL_RPATH=${SUPERBUILD_INSTALL_PREFIX}/lib;@rpath
      -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
    INSTALL_DIR "${SUPERBUILD_INSTALL_PREFIX}/VTK"
    )
  set( VTK_DIR "${base}/VTK-Build" )

  set( VMTK_DEPENDS ${VMTK_DEPENDS} "VTK" )

endif()

##
## VMTK - Normal Build
##
set( proj VMTK )
ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  BINARY_DIR VMTK-Build
  BUILD_ALWAYS 1 # always run this step. do not generate stamp file 
  CMAKE_GENERATOR ${gen}
  USES_TERMINAL_CONFIGURE 1
  USES_TERMINAL_BUILD 1
  USES_TERMINAL_INSTALL 1
  CMAKE_ARGS
    -DBUILDNAME:STRING=${BUILDNAME}
    -DSITE:STRING=${SITE}
    -DMAKECOMMAND:STRING=${MAKECOMMAND}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DBUILD_EXAMPLES:BOOL=${BUILD_EXAMPLES}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -DBUILD_DOCUMENTATION:BOOL=${BUILD_DOCUMENTATION}
    -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
    -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${SUPERBUILD_INSTALL_PREFIX}
    -DVTK_VMTK_WRAP_PYTHON:BOOL=${VTK_VMTK_WRAP_PYTHON}
    -DVMTK_USE_SUPERBUILD:BOOL=OFF
    -DVMTK_CONTRIB_SCRIPTS:BOOL=${VMTK_CONTRIB_SCRIPTS}
    -DVTK_VMTK_CONTRIB:BOOL=${VTK_VMTK_CONTRIB}
    -DVMTK_SCRIPTS_ENABLED:BOOL=${VMTK_SCRIPTS_ENABLED}
    -DVTK_ENABLE_VTKPYTHON:BOOL=ON
    -DVMTK_MINIMAL_INSTALL:BOOL=OFF
    -DVMTK_ENABLE_DISTRIBUTION:BOOL=${VMTK_ENABLE_DISTRIBUTION}
    -DVMTK_WITH_LIBRARY_VERSION:BOOL=OFF
    -DVMTK_BUILD_TETGEN:BOOL=${VMTK_BUILD_TETGEN}
    -DVTK_VMTK_USE_COCOA:BOOL=${VTK_VMTK_USE_COCOA}
    -DVMTK_BUILD_STREAMTRACER:BOOL=${VMTK_BUILD_STREAMTRACER}
    -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=${VTK_REQUIRED_OBJCXX_FLAGS}
    -DVMTK_USE_RENDERING:STRING=${VMTK_USE_RENDERING}
    -DITK_DIR:PATH=${ITK_DIR}
    -DVTK_DIR:PATH=${VTK_DIR}
    -DCMAKE_RC_COMPILER:BOOL=${CMAKE_RC_COMPILER}
    -DPython${Python3_VERSION_MAJOR}_ROOT_DIR=${Python3_ROOT_DIR}
    -DPython${Python3_VERSION_MAJOR}_DEBUG_LIBRARY=${Python3_DEBUG_LIBRARY}
    -DPython${Python3_VERSION_MAJOR}_EXECUTABLE=${Python3_EXECUTABLE}
    -DPython${Python3_VERSION_MAJOR}_INCLUDE_DIR=${Python3_DEBUG_LIBRARY}
    -DPython${Python3_VERSION_MAJOR}_LIBRARY=${Python3_LIBRARY}
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF
    -DCMAKE_MACOSX_RPATH=ON
    -DCMAKE_INSTALL_RPATH=${SUPERBUILD_INSTALL_PREFIX}/lib;@rpath
    -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
  INSTALL_DIR "${SUPERBUILD_INSTALL_PREFIX}/vmtk"
  DEPENDS
    ${VMTK_DEPENDS}
 )

set( VMTK_INSTALL_DIR ${CMAKE_BINARY_DIR}/Install )
file( TO_NATIVE_PATH ${VMTK_INSTALL_DIR} VMTK_INSTALL_DIR_NATIVE )

if (WIN32)
  #find_package( PYTHONINTERP )
  #find_package( PYTHONLIBS )
  #
  #set( PYTHON_MAJORMINOR ${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR} )
  #string( REGEX MATCH "[0-9][0-9]" PYTHON_MIN_MINOR ${PYTHON_EXECUTABLE}  )
  #string( REGEX MATCH "(.*[/])*" PYTHON_ROOT_DIR ${PYTHON_EXECUTABLE}  )
  #file( TO_NATIVE_PATH ${PYTHON_ROOT_DIR} PYTHON_ROOT_DIR_NATIVE )

  configure_file( vmtk_startup.bat.in ${VMTK_INSTALL_DIR}/vmtk_startup.bat )
else ()
  if (APPLE)
    set( LIBRARY_PATH_ENV_VAR "DYLD_LIBRARY_PATH")
  else ()
    set( LIBRARY_PATH_ENV_VAR "LD_LIBRARY_PATH")
  endif ()
  configure_file( vmtk_env.sh.in ${VMTK_INSTALL_DIR}/vmtk_env.sh )
endif ()
