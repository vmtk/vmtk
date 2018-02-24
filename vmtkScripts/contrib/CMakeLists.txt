PROJECT(VMTK_CONTRIB_SCRIPTS)

SET(CONTRIB_SCRIPTS_SRCS
    vmtkboundarylayer2.py
    vmtkcenterlinestonumpy.py
    vmtkdijkstradistancetopoints.py
    vmtkdistancetospheres.py
    vmtkgeodesicsurfaceresolution.py
    vmtkimagetonumpy.py
    vmtkmeshaddexternallayer.py
    vmtkmeshclipcenterlines.py
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
    vmtksurfaceresolution.py
    vmtksurfacetonumpy.py
    vmtksurfacewriter2.py
    vmtksurfaceextractannularwalls.py
    vmtksurfaceextractinnercylinder.py
    vmtkthreshold.py
    vmtkmeshmerge.py
    vmtkentityrenumber.py
  )
  
SET(CONTRIB_MODULE_SRCS
  vmtkcontribscripts.py
  )

IF(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR)
  SET(VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR bin)
ENDIF(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR)

IF(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR)
  SET(VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
ENDIF(NOT VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR)

FOREACH (SCRIPT_FILE  ${CONTRIB_SCRIPTS_SRCS})
  CONFIGURE_FILE(${VMTK_CONTRIB_SCRIPTS_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
ENDFOREACH (SCRIPT_FILE)

INSTALL(FILES ${CONTRIB_SCRIPTS_SRCS} ${CONTRIB_MODULE_SRCS}
  DESTINATION ${VMTK_CONTRIB_SCRIPTS_INSTALL_LIB_DIR}
  COMPONENT PythonRuntimeLibraries)

IF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

SET (STRIPPED_CONTRIB_SCRIPTS_SRCS )
FOREACH (SCRIPT_FILE  ${CONTRIB_SCRIPTS_SRCS})
  STRING(REGEX REPLACE ".py$" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
  CONFIGURE_FILE(${PYPES_SOURCE_DIR}/pyperun.py ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
  SET (CONTRIB_STRIPPED_SCRIPTS_SRCS ${CONTRIB_STRIPPED_SCRIPTS_SRCS} ${VMTK_CONTRIB_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
ENDFOREACH (SCRIPT_FILE)

INSTALL(PROGRAMS ${CONTRIB_STRIPPED_SCRIPTS_SRCS}
  DESTINATION ${VMTK_CONTRIB_SCRIPTS_INSTALL_BIN_DIR}
  COMPONENT RuntimeExecutables)

ENDIF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

