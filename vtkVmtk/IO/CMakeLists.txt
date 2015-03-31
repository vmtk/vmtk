set(VTK_VMTK_IO_TARGET_LINK_LIBRARIES )

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  set( VTK_VMTK_IO_COMPONENTS
    vtkCommonCore
    vtkIOImage
    vtkIOLegacy
    vtkImagingCore
    vtkDICOMParser
    )
  if (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
    list(APPEND VTK_VMTK_IO_COMPONENTS
      vtkWrappingPythonCore
      )
  endif(VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  find_package( VTK 6.2 COMPONENTS ${VTK_VMTK_IO_COMPONENTS} REQUIRED )
  include( ${VTK_USE_FILE} )
  list( APPEND VTK_VMTK_IO_TARGET_LINK_LIBRARIES ${VTK_LIBRARIES} )
else( ${VTK_VERSION_MAJOR} GREATER 5 )
  list( APPEND VTK_VMTK_IO_TARGET_LINK_LIBRARIES
    vtkCommon
    vtkImaging
    vtkIO
    vtkDICOMParser
    )
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

SET (VTK_VMTK_IO_SRCS
  vtkvmtkDICOMImageReader.cxx
  vtkvmtkDolfinWriter.cxx
  vtkvmtkFDNEUTReader.cxx
  vtkvmtkFDNEUTWriter.cxx
  vtkvmtkFluentWriter.cxx
  vtkvmtkPNGWriter.cxx
  vtkvmtkTetGenReader.cxx
  vtkvmtkTetGenWriter.cxx
  vtkvmtkXdaReader.cxx
  vtkvmtkXdaWriter.cxx
  )

if( ${VTK_VERSION_MAJOR} GREATER 5 )
  # XXX Ensure DICOMParser directory provided by VTK is included before
  #     the one provided by ITK.
  include_directories(BEFORE ${vtkDICOMParser_INCLUDE_DIRS})
else( ${VTK_VERSION_MAJOR} GREATER 5 )
IF (EXISTS ${VTK_DIR}/bin)
  #this is a hack to make DICOMParser be included correctly - VTK source
  INCLUDE_DIRECTORIES(BEFORE ${VTK_SOURCE_DIR}/Utilities/DICOMParser)
ELSE (EXISTS ${VTK_DIR}/bin)
  INCLUDE_DIRECTORIES(BEFORE ${VTK_INCLUDE_DIRS})
ENDIF (EXISTS ${VTK_DIR}/bin)
endif( ${VTK_VERSION_MAJOR} GREATER 5 )

set(lib_name vtkvmtkIO)

ADD_LIBRARY (${lib_name} ${VTK_VMTK_IO_SRCS})
IF(VMTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${VMTK_LIBRARY_PROPERTIES})
ENDIF(VMTK_LIBRARY_PROPERTIES)
SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(${lib_name} ${VTK_VMTK_IO_TARGET_LINK_LIBRARIES})
  
INSTALL(TARGETS ${lib_name}
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

#FILE(GLOB files "${VTK_VMTK_IO_SRCS}/*.h")
FILE(GLOB files "${VTK_VMTK_SOURCE_DIR}/IO/*.h")
INSTALL(FILES ${files}
  DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

IF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${VTK_VMTK_IO_SRCS}
    KIT_PYTHON_LIBRARIES vtkvmtkMiscPythonD
    KIT_INSTALL_BIN_DIR ${VTK_VMTK_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${VTK_VMTK_INSTALL_LIB_DIR}
    )
ENDIF (VTK_WRAP_PYTHON AND VTK_VMTK_WRAP_PYTHON)
