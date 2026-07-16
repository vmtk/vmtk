## Program: VMTK
## Language:  Python
## Date:      April 9, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkimagevolumeviewer as volumeviewer
import copy


@pytest.fixture(scope='module')
def expected_results_dict(input_datadir):
    from json import load
    import os
    with open(os.path.join(input_datadir, 'vmtkimagevolumeviewer_testresults.json'), 'r') as f:
        resultDict = load(f)
    return resultDict


@pytest.mark.parametrize('preset', [
    ("CT-AAA"), ("CT-AAA2"), ("CT-Bone"),
    ("CT-Bones"), ("CT-Cardiac"), ("CT-Cardiac2"),
    ("CT-Cardiac3"), ("CT-Chest-Contrast-Enhanced"), ("CT-Chest-Vessels"),
    ("CT-Coronary-Arteries"), ("CT-Coronary-Arteries-2"), ("CT-Coronary-Arteries-3"),
    ("CT-Cropped-Volume-Bone"), ("CT-Fat"), ("CT-Liver-Vasculature"),
    ("CT-Lung"), ("CT-MIP"), ("CT-Muscle"), ("CT-Pulmonary-Arteries"),
    ("CT-Soft-Tissue"), ("MR-Angio"), ("MR-Default"),
    ("MR-MIP"), ("MR-T2-Brain")
])
def test_volume_viewer_preset_values(aorta_image, preset, expected_results_dict):
    view = volumeviewer.vmtkImageVolumeViewer()
    view.Image = aorta_image
    view.Display = 0
    view.Preset = preset
    view.Execute()

    interpolationType = view.InterpolationType
    shade = view.Shade
    specularPower = view.SpecularPower
    specular = view.Specular
    diffuse = view.Diffuse
    ambient = view.Ambient

    colorTransferFunctionPointMin = view.ColorTransferFunction.GetRange()[0]
    colorTransferFunctionPointMax = view.ColorTransferFunction.GetRange()[1]
    colorTransferFunctionRangeWidth = colorTransferFunctionPointMax - colorTransferFunctionPointMin
    colorTransferFunctionSamplePointSmall = colorTransferFunctionPointMin + (colorTransferFunctionRangeWidth * 0.1)
    colorTransferFunctionSamplePointLarge = colorTransferFunctionPointMax - (colorTransferFunctionRangeWidth * 0.1)
    colorTransferFunctionColorAtSamplePointSmall = view.ColorTransferFunction.GetColor(colorTransferFunctionSamplePointSmall)
    colorTransferFunctionColorAtSamplePointLarge = view.ColorTransferFunction.GetColor(colorTransferFunctionSamplePointLarge)

    opacityTransferFunctionPointMin = view.OpacityTransferFunction.GetRange()[0]
    opacityTransferFunctionPointMax = view.OpacityTransferFunction.GetRange()[1]
    opacityTransferFunctionRangeWidth = opacityTransferFunctionPointMax - opacityTransferFunctionPointMin
    opacityTransferFunctionSamplePointSmall = opacityTransferFunctionPointMin + (opacityTransferFunctionRangeWidth * 0.1)
    opacityTransferFunctionSamplePointLarge = opacityTransferFunctionPointMax - (opacityTransferFunctionRangeWidth * 0.1)
    opacityTransferFunctionColorAtSamplePointSmall = view.OpacityTransferFunction.GetValue(opacityTransferFunctionSamplePointSmall)
    opacityTransferFunctionColorAtSamplePointLarge = view.OpacityTransferFunction.GetValue(opacityTransferFunctionSamplePointLarge)

    gradientOpacityTransferFunctionPointMin = view.GradientOpacityTransferFunction.GetRange()[0]
    gradientOpacityTransferFunctionPointMax = view.GradientOpacityTransferFunction.GetRange()[1]
    gradientOpacityTransferFunctionRangeWidth = gradientOpacityTransferFunctionPointMax - gradientOpacityTransferFunctionPointMin
    gradientOpacityTransferFunctionSamplePointSmall = gradientOpacityTransferFunctionPointMin + (gradientOpacityTransferFunctionRangeWidth * 0.1)
    gradientOpacityTransferFunctionSamplePointLarge = gradientOpacityTransferFunctionPointMax - (gradientOpacityTransferFunctionRangeWidth * 0.1)
    gradientOpacityTransferFunctionColorAtSamplePointSmall = view.GradientOpacityTransferFunction.GetValue(gradientOpacityTransferFunctionSamplePointSmall)
    gradientOpacityTransferFunctionColorAtSamplePointLarge = view.GradientOpacityTransferFunction.GetValue(gradientOpacityTransferFunctionSamplePointLarge)

    resultsDict = copy.deepcopy(expected_results_dict[preset])

    # The sample points and interpolated transfer-function values are computed in
    # floating point (scaling by 0.1, VTK's piecewise interpolation), so their last
    # bit differs between platforms' math libraries. Compare the numeric results with
    # pytest.approx rather than exact equality; the categorical InterpolationType and
    # Shade stay exact.
    assert interpolationType == resultsDict["InterpolationType"]
    assert shade == resultsDict["Shade"]
    assert specularPower == pytest.approx(resultsDict["SpecularPower"])
    assert specular == pytest.approx(resultsDict["Specular"])
    assert diffuse == pytest.approx(resultsDict["Diffuse"])
    assert ambient == pytest.approx(resultsDict["Ambient"])
    assert colorTransferFunctionPointMin == pytest.approx(resultsDict["ColorTransferFunctionPointMin"])
    assert colorTransferFunctionPointMax == pytest.approx(resultsDict["ColorTransferFunctionPointMax"])
    assert colorTransferFunctionRangeWidth == pytest.approx(resultsDict["ColorTransferFunctionRangeWidth"])
    assert colorTransferFunctionSamplePointSmall == pytest.approx(resultsDict["ColorTransferFunctionSamplePointSmall"])
    assert colorTransferFunctionSamplePointLarge == pytest.approx(resultsDict["ColorTransferFunctionSamplePointLarge"])
    assert colorTransferFunctionColorAtSamplePointSmall == pytest.approx(tuple(resultsDict["ColorTransferFunctionColorAtSamplePointSmall"]))
    assert colorTransferFunctionColorAtSamplePointLarge == pytest.approx(tuple(resultsDict["ColorTransferFunctionColorAtSamplePointLarge"]))
    assert opacityTransferFunctionPointMin == pytest.approx(resultsDict["OpacityTransferFunctionPointMin"])
    assert opacityTransferFunctionPointMax == pytest.approx(resultsDict["OpacityTransferFunctionPointMax"])
    assert opacityTransferFunctionRangeWidth == pytest.approx(resultsDict["OpacityTransferFunctionRangeWidth"])
    assert opacityTransferFunctionSamplePointSmall == pytest.approx(resultsDict["OpacityTransferFunctionSamplePointSmall"])
    assert opacityTransferFunctionSamplePointLarge == pytest.approx(resultsDict["OpacityTransferFunctionSamplePointLarge"])
    assert opacityTransferFunctionColorAtSamplePointSmall == pytest.approx(resultsDict["OpacityTransferFunctionColorAtSamplePointSmall"])
    assert opacityTransferFunctionColorAtSamplePointLarge == pytest.approx(resultsDict["OpacityTransferFunctionColorAtSamplePointLarge"])
    assert gradientOpacityTransferFunctionPointMin == pytest.approx(resultsDict["GradientOpacityTransferFunctionPointMin"])
    assert gradientOpacityTransferFunctionPointMax == pytest.approx(resultsDict["GradientOpacityTransferFunctionPointMax"])
    assert gradientOpacityTransferFunctionRangeWidth == pytest.approx(resultsDict["GradientOpacityTransferFunctionRangeWidth"])
    assert gradientOpacityTransferFunctionSamplePointSmall == pytest.approx(resultsDict["GradientOpacityTransferFunctionSamplePointSmall"])
    assert gradientOpacityTransferFunctionSamplePointLarge == pytest.approx(resultsDict["GradientOpacityTransferFunctionSamplePointLarge"])
    assert gradientOpacityTransferFunctionColorAtSamplePointSmall == pytest.approx(resultsDict["GradientOpacityTransferFunctionColorAtSamplePointSmall"])
    assert gradientOpacityTransferFunctionColorAtSamplePointLarge == pytest.approx(resultsDict["GradientOpacityTransferFunctionColorAtSamplePointLarge"])
