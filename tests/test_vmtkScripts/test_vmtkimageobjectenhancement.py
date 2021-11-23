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
import vmtk.vmtkimageobjectenhancement as enhance


def test_enhance_image_with_defaults(aorta_image, compare_images):
    name = __name__ + '_test_enhance_image_with_defaults.mha'
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


def test_enhance_image_with_scaled_objectiveness(aorta_image, compare_images):
    name = __name__ + '_test_enhance_image_with_scaled_objectiveness.mha'
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.ScaledObjectness = True
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


@pytest.mark.parametrize("sigma_min,sigma_max,paramid", [
    (0.5, 1.0, '0'),
    (1.0, 2.0, '1'),
    (2.0, 1.0, '2'),
    (0.7, 2.3, '3')
])
def test_enhance_image_with_changing_sigma(aorta_image, compare_images, sigma_min, sigma_max, paramid):
    name = __name__ + '_test_enhance_image_with_changing_sigma_' + paramid + '.mha'
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.SigmaMin = sigma_min
    enhancer.SigmaMax = sigma_max
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


@pytest.mark.parametrize("alpha,beta,gamma,paramid", [
    (0.5, 1.0, 5.0, '0'),
    (0.5, 0.5, 10.0, '1'),
    (2.0, 0.5, 10.0, '2'),
])
def test_enhance_image_with_changing_abg(aorta_image, compare_images, alpha, beta, gamma, paramid):
    name = __name__ + '_test_enhance_image_with_changing_abg_' + paramid + '.mha'
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.Alpha = alpha
    enhancer.Beta = beta
    enhancer.Gamma = gamma
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True


def test_enhance_image_three_sigma_steps(aorta_image, compare_images):
    name = __name__ + '_test_enhance_image_three_sigma_steps.mha'
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.NumberOfSigmaSteps = 3
    enhancer.Execute()

    assert compare_images(enhancer.Image, name) == True
