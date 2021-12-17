set(vtkvmtkITK_TARGET_LINK_LIBRARIES ${ITK_LIBRARIES})

set( vtkvmtkITK_COMPONENTS
  ${VTK_COMPONENT_PREFIX}IOImage
  ${VTK_COMPONENT_PREFIX}ImagingCore
  )
if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  list(APPEND vtkvmtkITK_COMPONENTS
    ${VTK_COMPONENT_PREFIX}WrappingPythonCore
    )
endif()
find_package( VTK ${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} COMPONENTS ${vtkvmtkITK_COMPONENTS} REQUIRED )
if(NOT VMTK_USE_VTK9)
  include( ${VTK_USE_FILE} )
endif()
list( APPEND vtkvmtkITK_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )

# --------------------------------------------------------------------------
# Sources

# include a test for endianness (used to be done by default)
include (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

set(vtkvmtkITK_SRCS
    vtkvmtkITKArchetypeImageSeriesReader.cxx
    vtkvmtkITKArchetypeImageSeriesScalarReader.cxx
    vtkvmtkITKImageWriter.cxx
    vtkvmtkITKVersion.cxx
)

set(VTK_VMTK_ITK_EXTRA_INCLUDES
  vtkvmtkITK.h
  vtkvmtkITKUtility.h
)

# --------------------------------------------------------------------------
# Build the library

vmtk_build_library(
  NAME vtkvmtkITK
  SRCS ${vtkvmtkITK_SRCS} ${VTK_VMTK_ITK_EXTRA_INCLUDES}
  TARGET_LINK_LIBRARIES ${vtkvmtkITK_TARGET_LINK_LIBRARIES}
  )
