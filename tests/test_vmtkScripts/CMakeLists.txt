set(TEST_VMTKSCRIPTS_SRCS
    conftest.py
    test_importvmtkscripts.py
    test_importvtkvmtk.py
    test_vmtkbifurcationprofiles.py
    test_vmtkbifurcationreferencesystems.py
    test_vmtkbifurcationsections.py
    test_vmtkbifurcationvectors.py
    test_vmtkbranchclipper.py
    test_vmtkbranchextractor.py
    test_vmtkbranchgeometry.py
    test_vmtkbranchmetrics.py
    test_vmtkbranchsections.py
    test_vmtkcenterlineattributes.py
    test_vmtkcenterlinegeometry.py
    test_vmtkcenterlineimage.py
    test_vmtkcenterlineinterpolation.py
    test_vmtkcenterlinemodeller.py
    test_vmtkcenterlineoffsetattributes.py
    test_vmtkcenterlineresampling.py
    test_vmtkcenterlines.py
    test_vmtkcenterlinesnetwork.py
    test_vmtkcenterlinesmoothing.py
    test_vmtkimagebinarize.py
    test_vmtkimagecast.py
    test_vmtkimagecompare.py
    test_vmtkimagecompose.py
    test_vmtkimagefeatures.py
    test_vmtkimageinitialization.py
    test_vmtkimagemorphology.py
    test_vmtkimagenormalize.py
    test_vmtkimageobjectenhancement.py
    test_vmtkimageotsuthresholds.py
    test_vmtkimagereader.py
    test_vmtkimageshiftscale.py
    test_vmtkimagesmoother.py
    test_vmtkimagevesselenhancement.py
    test_vmtkimagevoipainter.py
    test_vmtkimagevoiselector.py
    test_vmtkimagevolumeviewer.py
    test_vmtklevelsetsegmentation.py
    test_vmtkmarchingcubes.py
    # test_vmtkmeshtonumpy.py
    test_vmtksurfaceappend.py
    test_vmtksurfacebooleanoperation.py
    test_vmtksurfacecapper.py
    test_vmtksurfacecelldatatopointdata.py
    test_vmtksurfaceconnectivity.py
    test_vmtksurfacedecimation.py
    test_vmtksurfacekiteremoval.py
    test_vmtksurfacemassproperties.py
    test_vmtksurfacemodeller.py
    test_vmtksurfacenormals.py
    test_vmtksurfacereader.py
    test_vmtksurfaceremeshing.py
    test_vmtksurfacescaling.py
    test_vmtksurfacesmoothing.py
    test_vmtksurfacesubdivision.py
    test_vmtksurfacetobinaryimage.py
    test_vmtksurfacetransformtoras.py
    )

if(NOT TEST_VMTKSCRIPTS_INSTALL_LIB_DIR)
  set(TEST_VMTKSCRIPTS_INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/tests/test_vmtkScripts)
endif()

foreach (SCRIPT_FILE ${TEST_VMTKSCRIPTS_SRCS})
  configure_file(${VMTK_TESTING_SOURCE_DIR}/test_vmtkScripts/${SCRIPT_FILE} ${TEST_VMTKSCRIPTS_INSTALL_LIB_DIR}/${SCRIPT_FILE} @ONLY)
endforeach ()