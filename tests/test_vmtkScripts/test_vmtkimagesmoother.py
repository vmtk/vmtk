## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkimagesmoothing as imagesmoothing


def test_default_smoothing(aorta_image, compare_images):
    name = __name__ + '_test_default_smoothing.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True


def test_smoothing_high_standard_deviation(aorta_image, compare_images):
    name = __name__ + '_test_smoothing_high_standard_deviation.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.StandardDeviation = 7.6
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True


def test_lower_radius_factor(aorta_image, compare_images):
    name = __name__ + '_test_lower_radius_factor.mha'
    smoother = imagesmoothing.vmtkImageSmoothing()
    smoother.Image = aorta_image
    smoother.RadiusFactor = 3.4
    smoother.Execute()

    assert compare_images(smoother.Image, name) == True