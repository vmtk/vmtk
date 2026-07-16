project (TETGEN)

set (TETGEN_SRCS
    tetgen.cxx
    predicates.cxx
    )

add_definitions (-DTETLIBRARY)
include_directories(${TETGEN_SOURCE_DIR})
add_library (tet STATIC ${TETGEN_SRCS})

set_target_properties(tet PROPERTIES COMPILE_FLAGS "-O0")

if(NOT WIN32)
  set_target_properties(tet PROPERTIES COMPILE_FLAGS "-O0 -fPIC")
endif()

# The static TetGen library is linked into the vmtk libraries and is not
# needed at runtime, so it is excluded from Python wheels.
install(TARGETS tet DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} EXPORT VMTK-Targets ${VMTK_WHEEL_EXCLUDE_FROM_ALL})