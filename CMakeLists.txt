cmake_minimum_required(VERSION 3.12...3.29.1)

project(VMTK)

# vmtk versions number
set(VMTK_VERSION_MAJOR 1)
set(VMTK_VERSION_MINOR 5)
set(VMTK_VERSION_PATCH 0)
# Optional pre-release suffix used by the PyPI packages (setup.py appends it
# to the version, e.g. "a1" -> 1.5.0a1, "b2" -> 1.5.0b2, "rc1" -> 1.5.0rc1;
# see PEP 440). Leave empty for final releases. It intentionally does not
# affect VMTK_VERSION, which the C++ build uses for library versioning.
set(VMTK_VERSION_SUFFIX "")
set(VMTK_VERSION
    "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}.${VMTK_VERSION_PATCH}")

set(VMTK_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
add_subdirectory(CMake)
list(INSERT CMAKE_MODULE_PATH 0
  ${VMTK_CMAKE_DIR}
  ${vtkAddon_CMAKE_DIR}
  )

# Append the library version information to the library target properties.
option(VMTK_WITH_LIBRARY_VERSION "Build with library version information" OFF)
if(VMTK_WITH_LIBRARY_VERSION)
  # This setting of SOVERSION assumes that any API change
  # will increment either the minor or major version number of vmtk.
  set(VMTK_LIBRARY_PROPERTIES
      VERSION "${VMTK_VERSION}"
      SOVERSION "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}"
     )
endif()

find_package( Python3 COMPONENTS Interpreter )
if (NOT VMTK_PYTHON_VERSION)
  # find_package(Python3) sets Python3_VERSION_*; the PYTHON_VERSION_* naming
  # belongs to the legacy FindPythonInterp module and is never populated here,
  # which used to yield a "python." install directory unless the caller passed
  # VMTK_PYTHON_VERSION explicitly.
  set(VMTK_PYTHON_VERSION "python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}" CACHE STRING "" FORCE)
endif ()
mark_as_advanced(VMTK_PYTHON_VERSION)

if (APPLE AND VMTK_BREW_PYTHON)
  if(VMTK_BREW_PYTHON)
    option(VMTK_BREW_PYTHON "Link against Homebrew Python" ON)
    set(PYTHON_SHEBANG ${PYTHON_EXECUTABLE})
  else()
    set(PYTHON_SHEBANG "/usr/bin/env python")
  endif()
  #if(VMTK_BREW_PYTHON)
  #  execute_process(
  #    COMMAND /usr/local/bin/python -c "from distutils import sysconfig; print sysconfig.get_python_inc()"
  #    OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
  #    OUTPUT_STRIP_TRAILING_WHITESPACE)
  #  set(PYTHON_LIBRARY ${PYTHON_INCLUDE_DIR}../../lib/lib${VMTK_PYTHON_VERSION}.dylib)
  #endif()
elseif (UNIX)
  set(PYTHON_SHEBANG "/usr/bin/env python")
endif ()

option( VMTK_USE_X
  "Compile vmtk with X11." OFF )
if( VMTK_USE_X )
  set ( VTK_USE_X ON CACHE BOOL "" FORCE )
else ()
  set ( VTK_USE_X OFF CACHE BOOL "" FORCE )
endif()

#-----------------------------------------------------------------------------
# ITK setup
option( USE_SYSTEM_ITK
  "Exclude ITK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_ITK )
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
endif()

set(VMTK_RENDERING_BACKEND "OpenGL2" CACHE STRING "Graphic backend for VTK")

#-----------------------------------------------------------------------------
# VTK setup
option( USE_SYSTEM_VTK
  "Exclude VTK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_VTK )
  if( VMTK_WHEEL_BUILD )
    # When building against the VTK wheel SDK, a component-less
    # find_package(VTK) would try to resolve the third-party dependencies of
    # every module the SDK was built with (OpenXRRemoting, ...), most of
    # which are not available (and not needed). Only resolve a minimal
    # component here; each vtkVmtk kit finds its own VTK components.
    find_package( VTK REQUIRED COMPONENTS CommonCore )
  else()
    find_package( VTK REQUIRED )
  endif()
  if( ${VTK_VERSION_MAJOR} LESS 6 )
    include( ${VTK_USE_FILE} )
  endif()
endif()

# Superbuild setup
option( VMTK_USE_SUPERBUILD
  "Build VMTK and the projects it depends on via SuperBuild.cmake." ON )

set(VMTK_TEST_DATA_SOURCE "git-submodule" CACHE STRING "git-submodule initializes the vmtk-test-data repository. in-place assumes test data is already available.")
set_property(CACHE VMTK_TEST_DATA_SOURCE PROPERTY STRINGS in-place git-submodule)

# Git setup
if (VMTK_USE_SUPERBUILD OR VMTK_TEST_DATA_SOURCE MATCHES "git-submodule")
  if( NOT GIT_EXECUTABLE )
    find_package( Git REQUIRED )
  endif()

  option( GIT_PROTOCOL_HTTPS
    "Use HTTPS for git access (useful if behind a firewall)" OFF )
  if( GIT_PROTOCOL_HTTPS )
    set( GIT_PROTOCOL "https" CACHE STRING "Https protocol for file transfer" FORCE )
  else()
    set( GIT_PROTOCOL "git" CACHE STRING "Git protocol for file transfer" FORCE )
  endif()
  mark_as_advanced( GIT_PROTOCOL )
endif()

option( VMTK_USE_ITK5
  "Compile vmtk against ITK 5. OFF is ITK 4.13" ON )

option( VMTK_USE_VTK9
  "Compile vmtk against VTK 9. OFF is VTK 8" ON )

if( NOT VMTK_USE_VTK9 )
  message(FATAL_ERROR "This version of VMTK only supports VTK9")
endif()

option( VMTK_BUILD_TESTING
  "Build the VMTK testing libraries" OFF)

if(VMTK_BUILD_TESTING)
  add_subdirectory(tests)
endif()

option( VMTK_BUNJEE_BUILD
  "Build VMTK and the projects it depends on ready for being linked from Bunjee." OFF )

option( BUILD_VMTK_DOCUMENTATION "Build the methods to build doxygen documentation for VMTK locally" OFF )
mark_as_advanced(BUILD_VMTK_DOCUMENTATION)
if(BUILD_VMTK_DOCUMENTATION)
  set(BUILD_DOCUMENTATION ON)
endif()

if( VMTK_USE_SUPERBUILD )
  if( VMTK_BUNJEE_BUILD )
    set ( VTK_VMTK_WRAP_PYTHON OFF CACHE BOOL "" FORCE )
    set ( VMTK_CONTRIB_SCRIPTS OFF CACHE BOOL "" FORCE )
    # set ( VMTK_ENABLE_DISTRIBUTION OFF CACHE BOOL "" FORCE )
    #set ( VMTK_BUILD_TETGEN OFF CACHE BOOL "" FORCE )
    set ( VMTK_SCRIPTS_ENABLED OFF CACHE BOOL "" FORCE )
    set ( BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
    set ( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
    # set ( BUILD_TESTING OFF CACHE BOOL "" FORCE )
    set ( VTK_USE_COCOA ON )
    set ( VTK_VMTK_USE_COCOA ON )
    set ( VMTK_USE_RENDERING ON )
    set (  OFF )
    set ( VTK_WRAP_PYTHON OFF )
    set ( VTK_WRAP_TCL OFF )
    set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
    set ( ITK_NO_LIBRARY_VERSION ON )
    set ( VTK_NO_LIBRARY_VERSION ON )
    set ( VMTK_WITH_LIBRARY_VERSION OFF )
    set ( VTK_REQUIRED_OBJCXX_FLAGS "-fobjc-gc" CACHE STRING "" FORCE)
  else ()
    option ( VTK_VMTK_WRAP_PYTHON "Generate Python wrappings for C++ classes." ON )
    if (UNIX)
      set ( VTK_USE_X ON )
    endif ()
    if (APPLE)
      set ( VTK_USE_X OFF )
    endif()
    option ( VMTK_CONTRIB_SCRIPTS "Install modules from the vmtkScripts/contrib directory." ON )
    option ( VTK_VMTK_CONTRIB "Build and install classes in the vtkVmtk/Contrib directory." ON )
    # option ( VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF )
    option ( VMTK_BUILD_TETGEN "Build TetGen and TetGen wrapper. Check TetGen license before you activate this." ON )
    option ( VMTK_BUILD_STREAMTRACER "Build static temporal stream tracer." ON )
    if (APPLE)
      option ( VTK_VMTK_USE_COCOA "Build with Cocoa support." ON )
    endif ()
    set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
    option ( BUILD_SHARED_LIBS "Build shared libraries." ON )
    option ( VMTK_USE_RENDERING "Build VMTK rendering classes." ON )
    #option ( VTK_USE_HYBRID "Build VTK Hybrid classes." ON )
    option(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
    option(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)
  endif ()

  include( "${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake" )
  return()

else()

  if ( NOT ITK_FOUND )
    find_package( ITK REQUIRED )
    include( ${ITK_USE_FILE} )
  endif()

  if ( NOT VTK_FOUND )
    find_package( VTK REQUIRED )
    if( ${VTK_VERSION_MAJOR} LESS 6 )
      include( ${VTK_USE_FILE} )
    endif()
  endif()

endif()

#-----------------------------------------------------------------------------
# Python wheel build (PyPI packaging)
#
# Enabled by setup.py at the repository root (driven by scikit-build) to
# produce a binary wheel that works alongside the "vtk" wheels published on
# PyPI. The install tree is rearranged into a single self-contained "vmtk"
# Python package directory: pure Python modules, wrapped extension modules
# and native libraries are installed side by side so that relative rpaths
# (and, on Windows, os.add_dll_directory calls in the package __init__)
# resolve every dependency. Headers, CMake export/config files and
# command-line launcher scripts are excluded from the wheel; a "vmtk"
# console entry point defined in setup.py replaces the launchers.
option(VMTK_WHEEL_BUILD "Install a self-contained vmtk Python package layout for building a wheel." OFF)
mark_as_advanced(VMTK_WHEEL_BUILD)

# Expands to EXCLUDE_FROM_ALL inside install() commands that only make sense
# in a regular install tree (static libraries, import libraries): the files
# are then skipped by the component-less "cmake --install" that scikit-build
# performs, and stay out of the wheel.
set(VMTK_WHEEL_EXCLUDE_FROM_ALL "")

if(VMTK_WHEEL_BUILD)
  set(VMTK_WHEEL_EXCLUDE_FROM_ALL EXCLUDE_FROM_ALL)
  set(VMTK_WHEEL_PACKAGE_DIR vmtk)

  # The VTK::Python target exported by the VTK wheel SDK references
  # Python3::Python / Python3::Module in its link interface, and imported
  # targets created by find_package(Python3) are directory-scoped: the
  # per-kit find_package calls in the vtkVmtk subdirectories do not make
  # them visible at this scope, where the VTK targets are defined. Find
  # them here as well. Development.Embed (which provides Python3::Python)
  # is optional because some build environments (e.g. manylinux images)
  # do not ship an embeddable libpython.
  find_package(Python3 COMPONENTS Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.Embed)

  if(UNIX)
    # Wheel extension modules must not link libpython: the Python symbols
    # are provided by the running interpreter when the module is imported
    # (manylinux images do not even ship a libpython to link against).
    # vtkAddon's vtkWrapPython.cmake downgrades its
    # find_package(Python3 COMPONENTS Development) from REQUIRED to QUIET
    # when VTK_UNDEFINED_SYMBOLS_ALLOWED is enabled.
    set(VTK_UNDEFINED_SYMBOLS_ALLOWED ON)
    # scikit-build passes -DPYTHON_LIBRARY=... whenever the build Python
    # provides one (e.g. macOS framework builds), and vtkAddon's wrapping
    # links it into the wrapped modules; clear it so the modules resolve
    # the Python symbols at import time instead.
    set(PYTHON_LIBRARY "" CACHE FILEPATH
      "Cleared for the wheel build: modules must not link libpython" FORCE)
    set(PYTHON_DEBUG_LIBRARY "" CACHE FILEPATH
      "Cleared for the wheel build: modules must not link libpython" FORCE)
    if(APPLE)
      # Allow the wrapped MODULE libraries to link with unresolved Python
      # symbols (the default macOS module link flags treat them as errors).
      set(CMAKE_MODULE_LINKER_FLAGS
        "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
    endif()
  endif()

  # Everything that is needed at runtime goes into the package directory.
  set(VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(PYPES_MODULE_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(VMTK_SCRIPTS_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(VTK_VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(VTK_VMTK_INSTALL_LIB_DIR ${VMTK_WHEEL_PACKAGE_DIR})
  set(VTK_VMTK_INSTALL_BIN_DIR ${VMTK_WHEEL_PACKAGE_DIR})

  # No per-script launchers in the wheel (see the console entry point in
  # setup.py). This must be set before the PypeS/vmtkScripts subdirectories
  # are added; the option() declared later keeps this value (CMP0077).
  set(VMTK_MINIMAL_INSTALL ON)

  # Shared libraries and Python extension modules must find their
  # dependencies relative to the package directory: vmtk's own libraries
  # sit side by side, and the VTK libraries are provided by the "vtk" wheel
  # in the sibling "vtkmodules" package. VMTK_WHEEL_ITK_RUNTIME_LIB_DIR
  # (set by setup.py on platforms where ITK is built as shared libraries)
  # additionally lets the build-tree ITK libraries be found, both when the
  # wheel repair tool (auditwheel/delocate) resolves and bundles them and
  # when a locally built wheel is used directly.
  if(APPLE)
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
    # Use @loader_path (not @rpath) install names for the vmtk libraries:
    # every consumer (the wrapped extension modules and the other vmtk
    # libraries) lives in the same package directory, so the references
    # resolve without relying on LC_RPATH entries, which the wheel repair
    # step (delocate) may rewrite or remove.
    set(CMAKE_INSTALL_NAME_DIR "@loader_path")
    set(CMAKE_MACOSX_RPATH ON)
    list(APPEND CMAKE_INSTALL_RPATH
      "@loader_path"
      "@loader_path/../vtkmodules"
      "@loader_path/../vtkmodules/.dylibs"
      )
  elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH
      "$ORIGIN"
      "$ORIGIN/../vtkmodules"
      )
  endif()
  if(UNIX AND VMTK_WHEEL_ITK_RUNTIME_LIB_DIR)
    list(APPEND CMAKE_INSTALL_RPATH "${VMTK_WHEEL_ITK_RUNTIME_LIB_DIR}")
  endif()
endif()

option(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
mark_as_advanced(VMTK_SCRIPTS_ENABLED)

if(VMTK_SCRIPTS_ENABLED)
  set(vmtk_directories
    PypeS
    vtkVmtk
    vmtkScripts
    )
else()
  set(vmtk_directories
    vtkVmtk
    )
endif()

foreach(vmtk_directory ${vmtk_directories})
  add_subdirectory(${vmtk_directory})
endforeach()

option(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)

set(LIBRARY_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

set(VMTK_SRCS
   vmtk.py
   )

set(VMTK_MODULE_SRCS
   __init__.py.in
   )

if(NOT VMTK_INSTALL_BIN_DIR)
  set(VMTK_INSTALL_BIN_DIR bin)
endif()

if(NOT VMTK_INSTALL_LIB_DIR)
  set(VMTK_INSTALL_LIB_DIR lib)
endif()

if(NOT VMTK_MODULE_INSTALL_LIB_DIR)
  set(VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_INSTALL_LIB_DIR}/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
endif()

if(VMTK_SCRIPTS_ENABLED)

  foreach (SCRIPT_FILE ${VMTK_SRCS})
    configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
  endforeach ()

  configure_file(${VMTK_SOURCE_DIR}/__init__.py.in ${VMTK_BINARY_DIR}/__init__.py COPYONLY)

  if(VMTK_WHEEL_BUILD)
    # The wheel variant of the package __init__ registers the DLL
    # directories that Windows needs to load the wrapped extension modules.
    install(FILES ${VMTK_SOURCE_DIR}/distribution/pypi/package_init.py
      DESTINATION ${VMTK_MODULE_INSTALL_LIB_DIR}
      COMPONENT PythonRuntimeLibraries
      RENAME __init__.py)
  else()
    install(FILES __init__.py.in
      DESTINATION ${VMTK_MODULE_INSTALL_LIB_DIR}
      COMPONENT PythonRuntimeLibraries
      RENAME __init__.py)
  endif()

  if (VMTK_WHEEL_BUILD)
    # The "vmtk" console entry point declared in setup.py replaces the
    # vmtk launcher script.
  elseif (NOT WIN32)

    set (STRIPPED_SCRIPTS_SRCS )

    foreach (SCRIPT_FILE ${VMTK_SRCS})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      string(REGEX REPLACE ".py" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
      set (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    endforeach ()

    install(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  else ()

    foreach (SCRIPT_FILE ${VMTK_SRCS})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      string(REGEX REPLACE ".py" "-exe.py" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      configure_file(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
      set (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    endforeach ()

    install(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)

  endif ()

  # option(VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF)
  # if(VMTK_ENABLE_DISTRIBUTION)
  #   add_subdirectory(distribution)
  # endif()

  if(NOT VMTK_WHEEL_BUILD)
  install(EXPORT VMTK-Targets DESTINATION ${VMTK_BINARY_DIR})

  get_property(_wrap_hierarchy_targets GLOBAL PROPERTY VMTK_WRAP_HIERARCHY_TARGETS)
  if(_wrap_hierarchy_targets)
    foreach(target ${_wrap_hierarchy_targets})
      set(VMTK_WRAP_HIERARCHY_FILES_CONFIG
  "${VMTK_WRAP_HIERARCHY_FILES_CONFIG}
  set(${target}_WRAP_HIERARCHY_FILE
    \"${${target}_WRAP_HIERARCHY_FILE}\")"
  )
    endforeach()
  endif()

  configure_file(
    ${VMTK_SOURCE_DIR}/CMake/VMTKConfig.cmake.in
    ${VMTK_BINARY_DIR}/VMTKConfig.cmake
  @ONLY IMMEDIATE
  )
  install(FILES ${VMTK_BINARY_DIR}/VMTKConfig.cmake
    TYPE LIB
  )

  configure_file(
    ${VMTK_SOURCE_DIR}/CMake/VMTKUse.cmake.in
    ${VMTK_BINARY_DIR}/VMTKUse.cmake
  @ONLY IMMEDIATE
  )
  install(FILES ${VMTK_BINARY_DIR}/VMTKUse.cmake
    TYPE LIB
  )

  endif() # NOT VMTK_WHEEL_BUILD

endif()
