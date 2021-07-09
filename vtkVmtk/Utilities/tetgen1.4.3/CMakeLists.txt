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

install(TARGETS tet DESTINATION ${VTK_VMTK_INSTALL_BIN_DIR} EXPORT VMTK-Targets)