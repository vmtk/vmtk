set (OPENNL_SRCS
    nl_single_file.c
    )

include_directories(${OPENNL_SOURCE_DIR})

add_library (nl STATIC ${OPENNL_SRCS})

if(NOT WIN32)
  set_target_properties(nl PROPERTIES COMPILE_FLAGS -fPIC)
endif()

# OpenNL is a private copy, used only inside VMTK: its symbols, among them the BLAS routines it
# carries (dgemm_, dtpsv_, ...), are not exported from the libraries it is linked into. Exported,
# they could be bound instead of those of another BLAS in the same process - as with SciPy's in
# 3D Slicer in a web browser (SlicerWeb), where all libraries share one symbol namespace.
set_target_properties(nl PROPERTIES C_VISIBILITY_PRESET hidden)

install(TARGETS nl
  EXPORT VMTK-Targets
  LIBRARY DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${VTK_VMTK_INSTALL_LIB_DIR} COMPONENT Development ${VMTK_WHEEL_EXCLUDE_FROM_ALL}
  RUNTIME DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} COMPONENT RuntimeExecutables
)

if(NOT VMTK_WHEEL_BUILD)
  install(FILES nl.h
    DESTINATION ${VTK_VMTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)
endif()


