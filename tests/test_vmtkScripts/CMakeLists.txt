set(TEST_VMTKSCRIPTS_SRCS
    conftest.py
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
    test_vmtklevelsetsegmentation.py
    test_vmtkmarchingcubes.py
    test_importvmtkscripts.py
    test_vmtksurfaceappend.py
    test_vmtksurfacebooleanoperation.py
    test_vmtksurfacecapper.py
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
    )

IF(NOT TEST_VMTKSCRIPTS_INSTALL_LIB_DIR)
  SET(TEST_VMTKSCRIPTS_INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/tests/test_vmtkScripts)
ENDIF(NOT TEST_VMTKSCRIPTS_INSTALL_LIB_DIR)

get_filename_component(_VMTKExternalData_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

FOREACH (SCRIPT_FILE ${TEST_VMTKSCRIPTS_SRCS})
  CONFIGURE_FILE(${VMTK_TESTING_SOURCE_DIR}/test_vmtkScripts/${SCRIPT_FILE} ${TEST_VMTKSCRIPTS_INSTALL_LIB_DIR}/${SCRIPT_FILE} @ONLY)
ENDFOREACH (SCRIPT_FILE)