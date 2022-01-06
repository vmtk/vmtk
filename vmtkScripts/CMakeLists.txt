project(VMTK_SCRIPTS)

set(SCRIPTS_SRCS
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
  vmtkcenterlinesnetwork.py
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
  vmtkimagevolumeviewer.py
  vmtkimagewriter.py
  vmtklevelsetsegmentation.py
  vmtklineartoquadratic.py
  vmtklineresampling.py
  vmtklocalgeometry.py
  vmtkmarchingcubes.py
  vmtkcenterlineimage.py
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
  vmtksurfaceconnectivityselector.py
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
  vmtksurfacetobinaryimage.py
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

set(MODULE_SRCS
  vmtkscripts.py
  )

option(VMTK_CONTRIB_SCRIPTS "Install modules from the vmtkScripts/contrib directory." OFF)

if (VMTK_CONTRIB_SCRIPTS)
  add_subdirectory(contrib)
endif ()

if(NOT VMTK_SCRIPTS_INSTALL_BIN_DIR)
  set(VMTK_SCRIPTS_INSTALL_BIN_DIR bin)
endif()

if(NOT VMTK_SCRIPTS_INSTALL_LIB_DIR)
  set(VMTK_SCRIPTS_INSTALL_LIB_DIR lib/${VMTK_PYTHON_VERSION}/site-packages/vmtk)
endif()

add_subdirectory(resources)

foreach (SCRIPT_FILE  ${SCRIPTS_SRCS})
  configure_file(${VMTK_SCRIPTS_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_SCRIPTS_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
endforeach ()

install(FILES ${SCRIPTS_SRCS} ${MODULE_SRCS}
  DESTINATION ${VMTK_SCRIPTS_INSTALL_LIB_DIR}
  COMPONENT PythonRuntimeLibraries)

if (NOT WIN32 AND NOT VMTK_MINIMAL_INSTALL)

set (STRIPPED_SCRIPTS_SRCS )
foreach (SCRIPT_FILE  ${SCRIPTS_SRCS})
  string(REGEX REPLACE ".py$" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
  configure_file(${PYPES_SOURCE_DIR}/pyperun.py ${VMTK_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
  set (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_SCRIPTS_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
endforeach ()

install(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
  DESTINATION ${VMTK_SCRIPTS_INSTALL_BIN_DIR}
  COMPONENT RuntimeExecutables)

endif ()

