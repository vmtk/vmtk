set(vtkvmtkITK_TARGET_LINK_LIBRARIES ${ITK_LIBRARIES})

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( vtkvmtkITK_COMPONENTS
    vtkIOImage
    vtkImagingCore
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND vtkvmtkITK_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${vtkvmtkITK_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND vtkvmtkITK_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND vtkvmtkITK_TARGET_LINK_LIBRARIES
    vtkImaging
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

# --------------------------------------------------------------------------
# Sources

# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

SET(vtkvmtkITK_SRCS
    vtkvmtkITKArchetypeImageSeriesReader.cxx
    vtkvmtkITKArchetypeImageSeriesScalarReader.cxx
    vtkvmtkITKImageWriter.cxx
    vtkvmtkITKImageToImageFilter.h
    vtkvmtkITKImageToImageFilter2DFF.h
    vtkvmtkITKImageToImageFilterFF.h
)

SET(VTK_VMTK_ITK_EXTRA_INCLUDES
  vtkvmtkITK.h
  vtkvmtkITKUtility.h
)

# --------------------------------------------------------------------------
# Build the library

set(lib_name vtkvmtkITK)

ADD_LIBRARY (${lib_name} ${vtkvmtkITK_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
IF(NOT WIN32)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(NOT WIN32)
TARGET_LINK_LIBRARIES(${lib_name} ${vtkvmtkITK_TARGET_LINK_LIBRARIES})


INSTALL(TARGETS ${lib_name}
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/Utilities/${lib_name}/*.h")
INSTALL(FILES ${files} ${VTK_VMTK_ITK_EXTRA_INCLUDES}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

# --------------------------------------------------------------------------
# Wrapping

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${vtkvmtkITK_SRCS}
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
