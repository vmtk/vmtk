#
# Configure output paths for libraries and executables.
#
set(LIBRARY_OUTPUT_PATH ${VTK_VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${VTK_VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#
# Try to find VTK and include its settings (otherwise complain)
#
if ( NOT VTK_FOUND )
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif ()

#
# Try to find ITK and include its settings (otherwise complain)
#
if ( NOT ITK_FOUND )
  find_package(ITK REQUIRED)
  include(${ITK_USE_FILE})
endif ()

#
# Build shared libs ?
#
# Defaults to the same VTK setting.
#

# Standard CMake option for building libraries shared or static by default.
option(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})
# Copy the CMake option to a setting with VTK_VMTK_ prefix for use in
# our project.  This name is used in vtkvmtkConfigure.h.in.
set(VTK_VMTK_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

# If this is a build tree, provide an option for putting
# this project's executables and libraries in with VTK's.
if (EXISTS ${VTK_DIR}/bin)
  option(USE_VTK_OUTPUT_PATHS
         "Use VTK's output directory for this project's executables and libraries."
         OFF)
  mark_as_advanced (USE_VTK_OUTPUT_PATHS)
  if (USE_VTK_OUTPUT_PATHS)
    set (LIBRARY_OUTPUT_PATH ${VTK_DIR}/bin)
    set (EXECUTABLE_OUTPUT_PATH ${VTK_DIR}/bin)
  endif ()
endif ()


#
# Wrap Tcl, Java, Python
#
# Rational: even if your VTK was wrapped, it does not mean that you want to
# wrap your own local classes.
# Default value is OFF as the VTK cache might have set them to ON but
# the wrappers might not be present (or yet not found).
#

#
# Python
#

if (VTK_WRAP_PYTHON)

  option(VTK_VMTK_WRAP_PYTHON
         "Wrap classes into the Python interpreted language."
         ON)

  if (VTK_VMTK_WRAP_PYTHON)
    set(VTK_WRAP_PYTHON3_INIT_DIR "${VTK_VMTK_SOURCE_DIR}/Wrapping")
    #include(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
    if (WIN32)
      if (NOT BUILD_SHARED_LIBS)
        message(FATAL_ERROR "Python support requires BUILD_SHARED_LIBS to be ON.")
        set (VTK_VMTK_CAN_BUILD 0)
      endif ()
    endif ()
  endif ()

else ()

  if (VTK_VMTK_WRAP_PYTHON)
    message("Warning. VTK_VMTK_WRAP_PYTHON is ON but the VTK version you have "
            "chosen has not support for Python (VTK_WRAP_PYTHON is OFF).  "
            "Please set VTK_VMTK_WRAP_PYTHON to OFF.")
    set (VTK_VMTK_WRAP_PYTHON OFF)
  endif ()

endif ()

#
# Java
#

if (VTK_WRAP_JAVA)

  option(VTK_VMTK_WRAP_JAVA
         "Wrap classes into the Java interpreted language."
         ON)

  if (VTK_VMTK_WRAP_JAVA)
    if (VMTK_USE_VTK9)
      message(FATAL_ERROR "Java wrapping of classes is not supported with VTK 9")
    endif ()
    set(VTK_WRAP_JAVA3_INIT_DIR "${VTK_VMTK_SOURCE_DIR}/Wrapping")
    include(${VTK_CMAKE_DIR}/vtkWrapJava.cmake)
    if (WIN32)
      if (NOT BUILD_SHARED_LIBS)
        message(FATAL_ERROR "Java support requires BUILD_SHARED_LIBS to be ON.")
        set (VTK_VMTK_CAN_BUILD 0)
      endif ()
    endif ()

    # Tell the java wrappers where to go.
    set(VTK_JAVA_HOME ${VTK_VMTK_BINARY_DIR}/java/vtkvmtk)
    make_directory(${VTK_JAVA_HOME})
  endif ()

else ()

  if (VTK_VMTK_WRAP_JAVA)
    message("Warning. VTK_VMTK_WRAP_JAVA is ON but the VTK version you have "
            "chosen has not support for Java (VTK_WRAP_JAVA is OFF).  "
            "Please set VTK_VMTK_WRAP_JAVA to OFF.")
    set (VTK_VMTK_WRAP_JAVA OFF)
  endif ()

endif ()

# setup our local hints file in case wrappers need them.
set(VTK_WRAP_HINTS ${VTK_VMTK_SOURCE_DIR}/Wrapping/hints)
