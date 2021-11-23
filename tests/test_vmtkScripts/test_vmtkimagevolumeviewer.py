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

    assert interpolationType == resultsDict["InterpolationType"]
    assert shade == resultsDict["Shade"]
    assert specularPower == resultsDict["SpecularPower"]
    assert specular == resultsDict["Specular"]
    assert diffuse == resultsDict["Diffuse"]
    assert ambient == resultsDict["Ambient"]
    assert colorTransferFunctionPointMin == resultsDict["ColorTransferFunctionPointMin"]
    assert colorTransferFunctionPointMax == resultsDict["ColorTransferFunctionPointMax"]
    assert colorTransferFunctionRangeWidth == resultsDict["ColorTransferFunctionRangeWidth"]
    assert colorTransferFunctionSamplePointSmall == resultsDict["ColorTransferFunctionSamplePointSmall"]
    assert colorTransferFunctionSamplePointLarge == resultsDict["ColorTransferFunctionSamplePointLarge"]
    assert colorTransferFunctionColorAtSamplePointSmall == tuple(resultsDict["ColorTransferFunctionColorAtSamplePointSmall"])
    assert colorTransferFunctionColorAtSamplePointLarge == tuple(resultsDict["ColorTransferFunctionColorAtSamplePointLarge"])
    assert opacityTransferFunctionPointMin == resultsDict["OpacityTransferFunctionPointMin"]
    assert opacityTransferFunctionPointMax == resultsDict["OpacityTransferFunctionPointMax"]
    assert opacityTransferFunctionRangeWidth == resultsDict["OpacityTransferFunctionRangeWidth"]
    assert opacityTransferFunctionSamplePointSmall == resultsDict["OpacityTransferFunctionSamplePointSmall"]
    assert opacityTransferFunctionSamplePointLarge == resultsDict["OpacityTransferFunctionSamplePointLarge"]
    assert opacityTransferFunctionColorAtSamplePointSmall == resultsDict["OpacityTransferFunctionColorAtSamplePointSmall"]
    assert opacityTransferFunctionColorAtSamplePointLarge == resultsDict["OpacityTransferFunctionColorAtSamplePointLarge"]
    assert gradientOpacityTransferFunctionPointMin == resultsDict["GradientOpacityTransferFunctionPointMin"]
    assert gradientOpacityTransferFunctionPointMax == resultsDict["GradientOpacityTransferFunctionPointMax"]
    assert gradientOpacityTransferFunctionRangeWidth == resultsDict["GradientOpacityTransferFunctionRangeWidth"]
    assert gradientOpacityTransferFunctionSamplePointSmall == resultsDict["GradientOpacityTransferFunctionSamplePointSmall"]
    assert gradientOpacityTransferFunctionSamplePointLarge == resultsDict["GradientOpacityTransferFunctionSamplePointLarge"]
    assert gradientOpacityTransferFunctionColorAtSamplePointSmall == resultsDict["GradientOpacityTransferFunctionColorAtSamplePointSmall"]
    assert gradientOpacityTransferFunctionColorAtSamplePointLarge == resultsDict["GradientOpacityTransferFunctionColorAtSamplePointLarge"]
