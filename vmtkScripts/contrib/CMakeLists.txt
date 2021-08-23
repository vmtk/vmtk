project(VMTK_CONTRIB_SCRIPTS)

set(CONTRIB_SCRIPTS_SRCS
    vmtkboundarylayer2.py
    vmtkcenterlinestonumpy.py
    vmtkdijkstradistancetopoints.py
    vmtkdistancetospheres.py
    vmtkendpointsections.py
    vmtkentityextractor.py
    vmtkentitylist.py
    vmtkentityrenumber.py
    vmtkgeodesicsurfaceresolution.py
    vmtkimagetonumpy.py
    vmtkmeshaddexternallayer.py
    vmtkmeshcapper.py
    vmtkmeshconnector.py
    vmtkmeshconnectorfsi.py
    vmtkmeshclipcenterlines.py
    vmtkmeshinfo.py
    vmtkmeshmerge.py
    vmtkmeshringgenerator.py
    vmtkmeshtethex.py
    vmtkmeshtetrahedralize2.py
    vmtkmeshtonumpy.py
    vmtkmeshviewer2.py
    vmtkmeshwriter2.py
    vmtknumpyreader.py
    vmtknumpytocenterlines.py
    vmtknumpytoimage.py
    vmtknumpytomesh.py
    vmtknumpytosurface.py
    vmtknumpywriter.py
    vmtksurfacebooleanconnector.py
    vmtksurfaceconnector.py
    vmtksurfacefeatureedges.py
    vmtksurfaceharmonicconnector.py
    vmtksurfaceharmonicextension.py
    vmtksurfaceharmonicsections.py
    vmtksurfaceharmonicsolver.py
    vmtksurfaceremeshingramp.py
    vmtksurfaceresolution.py
    vmtksurfacetagger.py
    vmtksurfacethickening.py
    vmtksurfacetonumpy.py
    vmtksurfacewriter2.py
    vmtksurfaceextractannularwalls.py
    vmtksurfaceextractinnercylinder.py
    vmtkthreshold.py
    vmtkpeterresurface.py
    vmtkpetergeneratesurface.py
    vmtkpetersurfaceclipper.py 
  )

set(CONTRIB_MODULE_SRCS
  vmtkcontribscripts.py
  )

if(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR)
  set(VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR bin)
endif()

if(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR)
  set(VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
endif()

foreach (SCRIPT_FILE  ${CONTRIB_SCRIPTS_SRCS})
  configure_file(${VMTK_CONTRIB_SCRIPTS_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
endforeach ()

install(FILES ${CONTRIB_SCRIPTS_SRCS} ${CONTRIB_MODULE_SRCS}
  DESTINATION ${VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR}
  COMPONENT PythonRuntimeLibraries)

if (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

set (STRIPPED_CONTRIB_SCRIPTS_SRCS )
foreach (SCRIPT_FILE  ${CONTRIB_SCRIPTS_SRCS})
  string(REGEX REPLACE ".py$" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
  configure_file(${PYPES_SOURCE_DIR}/pyperun.py ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
  set (CONTRIB_STRIPPED_SCRIPTS_SRCS ${CONTRIB_STRIPPED_SCRIPTS_SRCS} ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
endforeach ()

install(PROGRAMS ${CONTRIB_STRIPPED_SCRIPTS_SRCS}
  DESTINATION ${VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR}
  COMPONENT RuntimeExecutables)

endif ()

