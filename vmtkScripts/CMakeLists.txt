PROJECT(VMTK_SCRIPTS)

SET(SCRIPTS_SRCS
  __init__.py
  vmtkmeshcompare.py
  vmtkimagecompare.py
  vmtksurfacecompare.py
  vmtkactivetubes.py
  vmtkbifurcationprofiles.py
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
  vmtkcenterlineinterpolation.py
  vmtkcenterlinelabeler.py
  vmtkcenterlinemeshsections.py
  vmtkcenterlinemerge.py
  vmtkcenterlinemodeller.py
  vmtkcenterlineoffsetattributes.py
  vmtkcenterlineresampling.py
  vmtkcenterlines.py
  vmtkcenterlinesections.py
  vmtkcenterlinesmoothing.py
  vmtkcenterlineviewer.py
  vmtkdelaunayvoronoi.py
  vmtkdistancetocenterlines.py
  vmtkendpointextractor.py
  vmtkflowextensions.py
  vmtkicpregistration.py
  vmtkimagebinarize.py
  vmtkimagecast.py
  vmtkimagecompose.py
  vmtkimagecurvedmpr.py
  vmtkimagefeaturecorrection.py
  vmtkimagefeatures.py
  vmtkimageinitialization.py
  vmtkimagemipviewer.py
  vmtkimagemorphology.py
  vmtkimagenormalize.py
  vmtkimageobjectenhancement.py
  vmtkimageotsuthresholds.py
  vmtkimagereader.py
  vmtkimagereslice.py
  vmtkimageseeder.py
  vmtkimageshiftscale.py
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
  vmtkmesharrayoperation.py
  vmtkmeshboundaryinspector.py
  vmtkmeshbranchclipper.py
  vmtkmeshclipper.py
  vmtkmeshconnectivity.py
  vmtkmeshcutter.py
  vmtkmeshdatareader.py
  vmtkmeshextractpointdata.py
  vmtkmeshlambda2.py
  vmtkmeshlinearize.py
  vmtkmeshgenerator.py
  vmtkmeshmergetimesteps.py
  vmtkmeshpolyballevaluation.py
  vmtkmeshprojection.py
  vmtkmeshreader.py
  vmtkmeshscaling.py
  vmtkmeshtetrahedralize.py
  vmtkmeshtosurface.py
  vmtkmeshtransform.py
  vmtkmeshtransformtoras.py
  vmtkmeshvectorfromcomponents.py
  vmtkmeshviewer.py
  vmtkmeshvolume.py
  vmtkmeshvorticityhelicity.py
  vmtkmeshwallshearrate.py
  vmtkmeshwriter.py
  vmtknetworkextraction.py
  vmtknetworkeditor.py
  vmtknetworkwriter.py
  vmtkparticletracer.py
  vmtkpathlineanimator.py
  vmtkpointsplitextractor.py
  vmtkpointtransform.py
  vmtkpolyballmodeller.py
  vmtkpotentialfit.py
  vmtkpythonscript.py
  vmtkrenderer.py
  vmtkrendertoimage.py
  vmtkrbfinterpolation.py
  vmtksurfaceappend.py
  vmtksurfacearrayoperation.py
  vmtksurfacearraysmoothing.py
  vmtksurfacebooleanoperation.py
  vmtksurfacecapper.py
  vmtksurfacecelldatatopointdata.py
  vmtksurfacecenterlineprojection.py
  vmtksurfaceclipper.py
  vmtksurfacecliploop.py
  vmtksurfaceconnectivity.py
  vmtksurfacecurvature.py
  vmtksurfacedecimation.py
  vmtksurfacedistance.py
  vmtksurfaceendclipper.py
  vmtksurfacekiteremoval.py
  vmtksurfaceloopextraction.py
  vmtksurfacemassproperties.py
  vmtksurfacemodeller.py
  vmtksurfacenormals.py
  vmtksurfacepointdatatocelldata.py
  vmtksurfacepolyballevaluation.py
  vmtksurfaceprojection.py
  vmtksurfacereader.py
  vmtksurfacereferencesystemtransform.py
  vmtksurfaceregiondrawing.py
  vmtksurfaceremeshing.py
  vmtksurfacescaling.py
  vmtksurfacesmoothing.py
  vmtksurfacesubdivision.py
  vmtksurfacetransform.py
  vmtksurfacetransforminteractive.py
  vmtksurfacetransformtoras.py
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

OPTION(VMTK_CONTRIB_SCRIPTS "Install modules from the vmtkScripts/contrib directory." OFF)

IF (VMTK_CONTRIB_SCRIPTS)
  SUBDIRS(contrib)
ENDIF (VMTK_CONTRIB_SCRIPTS)

IF(NOT VMTK_SCRIPTS_INSTALL_BIN_DIR)
  SET(VMTK_SCRIPTS_INSTALL_BIN_DIR bin)
ENDIF(NOT VMTK_SCRIPTS_INSTALL_BIN_DIR)

IF(NOT VMTK_SCRIPTS_INSTALL_LIB_DIR)
  SET(VMTK_SCRIPTS_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
ENDIF(NOT VMTK_SCRIPTS_INSTALL_LIB_DIR)

FOREACH (SCRIPT_FILE  ${SCRIPTS_SRCS})
  CONFIGURE_FILE(${VMTK_SCRIPTS_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_SCRIPTS_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
ENDFOREACH (SCRIPT_FILE)

INSTALL(FILES ${SCRIPTS_SRCS} ${MODULE_SRCS}
  DESTINATION ${VMTK_SCRIPTS_INSTALL_LIB_DIR}
  COMPONENT PythonRuntimeLibraries)

IF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

SET (STRIPPED_SCRIPTS_SRCS )
FOREACH (SCRIPT_FILE  ${SCRIPTS_SRCS})
  STRING(REGEX REPLACE ".py$" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
  CONFIGURE_FILE(${PYPES_SOURCE_DIR}/pyperun.py ${VMTK_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
  SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
ENDFOREACH (SCRIPT_FILE)

INSTALL(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
  DESTINATION ${VMTK_SCRIPTS_INSTALL_BIN_DIR}
  COMPONENT RuntimeExecutables)

ENDIF (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

