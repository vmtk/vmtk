## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
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
import vmtk.vmtkimagesmoothing as imagesmoothing


# with default gauss smoothig
def test_default_smoothing(aorta_image, compare_images):
    name = __name__ + '_test_default_smoothing.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True

# with default gauss smoothing


def test_smoothing_high_standard_deviation(aorta_image, compare_images):
    name = __name__ + '_test_smoothing_high_standard_deviation.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.StandardDeviation = 7.6
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True

# with default gauss smoothing


def test_lower_radius_factor(aorta_image, compare_images):
    name = __name__ + '_test_lower_radius_factor.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.RadiusFactor = 3.4
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True


def test_anisotropic_smoothing_default_params(aorta_image, compare_images, write_image):
    name = __name__ + '_test_anisotropic_smoothing_default_params.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.Method = 'anisotropic'
    smoother.Execute()

    write_image(smoother.Image, name)
    assert compare_images(smoother.Image, name) == True


@pytest.mark.parametrize("TimeStep,AutoCalculateTimeStep,Conductance,NumberOfIterations,paramid", [
    (0.0450, 0, 1.0, 5, '0'),
    (0.0450, 1, 1.0, 5, '1'),
    (0.0625, 1, 2.0, 5, '2'),
    (0.0625, 1, 1.0, 3, '3'),
    (0.0625, 1, 2.0, 6, '4'),
])
def test_anisotropic_smoother_varied_params(aorta_image, compare_images,
                                            TimeStep, AutoCalculateTimeStep, Conductance,
                                            NumberOfIterations, paramid, write_image):
    name = __name__ + '_test_anisotropic_smoother_varied_params_' + paramid + '.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.Method = 'anisotropic'
    smoother.TimeStep = TimeStep
    smoother.AutoCalculateTimeStep = AutoCalculateTimeStep
    smoother.Conductance = Conductance
    smoother.NumberOfIterations = NumberOfIterations
    smoother.Execute()

    write_image(smoother.Image, name)
    assert compare_images(smoother.Image, name) == True
