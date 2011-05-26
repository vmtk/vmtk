##############################################################################
#
# Library:   VMTK
#
##############################################################################
include( ExternalProject )

set( base "${CMAKE_BINARY_DIR}" )
set_property( DIRECTORY PROPERTY EP_BASE ${base} )

set( shared ON ) # use for BUILD_SHARED_LIBS on all subsequent projects
set( testing OFF ) # use for BUILD_TESTING on all subsequent projects
set( build_type "Debug" )
if( CMAKE_BUILD_TYPE )
  set( build_type "${CMAKE_BUILD_TYPE}" )
endif()

set( VMTK_DEPENDS "" )

set( gen "${CMAKE_GENERATOR}" )

##
## Check if sytem ITK or superbuild ITK
##
if( NOT USE_SYSTEM_ITK )

  if( NOT GIT_EXECUTABLE )
    find_package( Git REQUIRED )
  endif( NOT GIT_EXECUTABLE )

  option( GIT_PROTOCOL_HTTPS 
    "Use HTTPS for git access (useful if behind a firewall)" OFF )
  if( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "https" CACHE STRING "Https protocol for file transfer" )
  else( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "git" CACHE STRING "Git protocol for file transfer" )
  endif( GIT_PROTOCOL_HTTPS )
  mark_as_advanced( GIT_PROTOCOL )

  ##
  ## ITK
  ##
  set( proj ITK )
  ExternalProject_Add( ${proj}
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/Kitware/ITK.git"
    GIT_TAG "origin/release"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/ITK"
    BINARY_DIR ITK-Build
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -Dgit_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
      -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/Install
      -DBUILD_SHARED_LIBS:BOOL=${shared}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DITK_USE_REVIEW:BOOL=ON
      -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON
    #INSTALL_DIR "Install/ITK"
    )
  set( ITK_DIR "${base}/ITK-Build" )

  set( VMTK_DEPENDS ${VMTK_DEPENDS} "ITK" )

endif( NOT USE_SYSTEM_ITK )

##
## Check if sytem VTK or superbuild VTK
##
if( NOT USE_SYSTEM_VTK )

  if( NOT GIT_EXECUTABLE )
    find_package( Git REQUIRED )
  endif( NOT GIT_EXECUTABLE )

  option( GIT_PROTOCOL_HTTPS 
    "Use HTTPS for git access (useful if behind a firewall)" OFF )
  if( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "https" CACHE STRING "Https protocol for file transfer" )
  else( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "git" CACHE STRING "Git protocol for file transfer" )
  endif( GIT_PROTOCOL_HTTPS )
  mark_as_advanced( GIT_PROTOCOL )

  ##
  ## VTK
  ##
  set( proj VTK )
  ExternalProject_Add( VTK
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/lantiga/VTK.git"
    GIT_TAG "origin/release"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/VTK"
    BINARY_DIR VTK-Build
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -Dgit_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
      -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/Install
      -DBUILD_SHARED_LIBS:BOOL=${shared}
      -DVTK_WRAP_PYTHON:BOOL=ON
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DVTK_USE_GUISUPPORT:BOOL=OFF
      -DVTK_INSTALL_PYTHON_USING_CMAKE:BOOL=ON
    #INSTALL_DIR "Install/VTK"
    )
  set( VTK_DIR "${base}/VTK-Build" )

  set( VMTK_DEPENDS ${VMTK_DEPENDS} "VTK" )

endif( NOT USE_SYSTEM_VTK )

##
## VMTK - Normal Build
##
set( proj VMTK )
ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  BINARY_DIR VMTK-Build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DBUILDNAME:STRING=${BUILDNAME}
    -DSITE:STRING=${SITE}
    -DMAKECOMMAND:STRING=${MAKECOMMAND}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=${shared}
    -DBUILD_EXAMPLES:BOOL=${BUILD_EXAMPLES}
    -DBUILD_TESTING:BOOL=${testing}
    -DBUILD_DOCUMENTATION:BOOL=${BUILD_DOCUMENTATION}
    -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
    -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/Install
    -DVTK_VMTK_WRAP_PYTHON:BOOL=ON
    -DVMTK_USE_SUPERBUILD:BOOL=OFF
    -DVMTK_CONTRIB_SCRIPTS:BOOL=ON
    -DVTK_VMTK_CONTRIB:BOOL=ON
    -DVMTK_MINIMAL_INSTALL:BOOL=OFF
    -DVMTK_ENABLE_DISTRIBUTION:BOOL=OFF
    -DVMTK_WITH_LIBRARY_VERSION:BOOL=OFF
    -DVMTK_BUILD_TETGEN:BOOL=ON
    -DITK_DIR:PATH=${ITK_DIR}
    -DVTK_DIR:PATH=${VTK_DIR}
  #INSTALL_DIR "Install/vmtk"
  DEPENDS
    ${VMTK_DEPENDS}
 )

