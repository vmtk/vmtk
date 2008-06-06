PROJECT(VMTK_SCRIPTS)

SET(SCRIPTS_SRCS
  vmtkbifurcationreferencesystems.py
  vmtkbifurcationsections.py
  vmtkbifurcationvectors.py
  vmtkboundarylayer.py
  vmtkboundaryreferencesystems.py
  vmtkbranchclipper.py
  vmtkbranchextractor.py
  vmtkbranchgeometry.py
  vmtkbranchmapping.py
  vmtkbranchmetrics.py
  vmtkbranchpatching.py
  vmtkbranchsections.py
  vmtkcenterlineattributes.py
  vmtkcenterlinegeometry.py
  vmtkcenterlinelabeler.py
  vmtkcenterlinemeshsections.py
  vmtkcenterlinemerge.py
  vmtkcenterlineoffsetattributes.py
  vmtkcenterlineresampling.py
  vmtkcenterlines.py
  vmtkcenterlinesections.py
  vmtkcenterlinesmoothing.py
  vmtkcenterlineviewer.py
  vmtkdistancetocenterlines.py
  vmtkendpointextractor.py
  vmtkflowextensions.py
  vmtkicpregistration.py
  vmtkimagecast.py
  vmtkimagecompose.py
  vmtkimagefeaturecorrection.py
  vmtkimagefeatures.py
  vmtkimageinitialization.py
  vmtkimagelinetracer.py
  vmtkimagemipviewer.py
  vmtkimagereader.py
  vmtkimagereslice.py
  vmtkimageseeder.py
  vmtkimagesmoothing.py
  vmtkimagevesselenhancement.py
  vmtkimageviewer.py
  vmtkimagevoipainter.py
  vmtkimagevoiselector.py
  vmtkimagewriter.py
  vmtklevelsetsegmentation.py
  vmtklineartoquadratic.py
  vmtklineresampling.py
  vmtklocalgeometry.py
  vmtkmarchingcubes.py
  vmtkmeshboundaryinspector.py
  vmtkmeshclipper.py
  vmtkmeshdatareader.py
  vmtkmeshlinearize.py
  vmtkmeshgenerator.py
  vmtkmeshprojection.py
  vmtkmeshreader.py
  vmtkmeshscaling.py
  vmtkmeshtosurface.py
  vmtkmeshviewer.py
  vmtkmeshwriter.py
  vmtkpointsplitextractor.py
  vmtkpointtransform.py
  vmtkpotentialfit.py
  vmtkpythonscript.py
  vmtkrenderer.py
  vmtksurfacecapper.py
  vmtksurfacecelldatatopointdata.py
  vmtksurfacecenterlineprojection.py
  vmtksurfaceclipper.py
  vmtksurfaceconnectivity.py
  vmtksurfacedecimation.py
  vmtksurfacedistance.py
  vmtksurfacekiteremoval.py
  vmtksurfacenormals.py
  vmtksurfaceprojection.py
  vmtksurfacereader.py
  vmtksurfacereferencesystemtransform.py
  vmtksurfaceremeshing.py
  vmtksurfacescaling.py
  vmtksurfacesmoothing.py
  vmtksurfacesubdivision.py
  vmtksurfacetransform.py
  vmtksurfacetriangle.py
  vmtksurfacetomesh.py
  vmtksurfaceviewer.py
  vmtksurfacewriter.py
  vmtksurfmesh.py
  vmtktetgen.py
  vmtktetringenerator.py
  )

SET(MODULE_SRCS
  vmtkscripts.py
  )

IF(NOT VMTK_SCRIPTS_INSTALL_BIN_DIR)
  SET(VMTK_SCRIPTS_INSTALL_BIN_DIR ${VMTK_SCRIPTS_INSTALL_ROOT}/bin)
ENDIF(NOT VMTK_SCRIPTS_INSTALL_BIN_DIR)

IF(NOT VMTK_SCRIPTS_INSTALL_LIB_DIR)
  SET(VMTK_SCRIPTS_INSTALL_LIB_DIR ${VMTK_SCRIPTS_INSTALL_ROOT}/lib/vmtk/vmtk)
ENDIF(NOT VMTK_SCRIPTS_INSTALL_LIB_DIR)

FOREACH (SCRIPT_FILE  ${SCRIPTS_SRCS})
  CONFIGURE_FILE(${VMTK_SCRIPTS_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_SCRIPTS_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
ENDFOREACH (SCRIPT_FILE)

INSTALL_FILES(${VMTK_SCRIPTS_INSTALL_LIB_DIR} .py ${SCRIPTS_SRCS} ${MODULE_SRCS})

IF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

SET (STRIPPED_SCRIPTS_SRCS )
FOREACH (SCRIPT_FILE  ${SCRIPTS_SRCS})
  STRING(REGEX REPLACE ".py$" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
  CONFIGURE_FILE(${PYPES_SOURCE_DIR}/pyperun.py ${VMTK_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
  SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${STRIPPED_SCRIPT_FILE})
ENDFOREACH (SCRIPT_FILE)

INSTALL_PROGRAMS(${VMTK_SCRIPTS_INSTALL_BIN_DIR} FILES ${STRIPPED_SCRIPTS_SRCS})

ENDIF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

