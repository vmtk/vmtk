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
import vmtk.vmtkimagefeatures as imagefeatures


#TODO: WHY DOES "fwhm" seg fault?
@pytest.mark.parametrize("featureType,paramid", [
    ("vtkgradient", '0'),
    ("gradient", '1'),
])
def test_features_types(aorta_image, compare_images, featureType, paramid, write_image):
    name = __name__ + '_test_features_types_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True


@pytest.mark.parametrize("featureType,paramid", [
    ("upwind", '2'),
])
def test_features_types_upwind(aorta_image, compare_images, featureType, paramid, write_image):
    name = __name__ + '_test_features_types_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True


@pytest.mark.parametrize("featureType,paramid", [
    ("gradient", '0'),
])
def test_sigmoid_on_for_gradient(aorta_image, compare_images, featureType, paramid, write_image):
    name = __name__ + '_test_sigmoid_on_for_gradient_and_upwind_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.SigmoidRemapping = 1
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True


@pytest.mark.parametrize("featureType,paramid", [
    ("upwind", '1'),
])
def test_sigmoid_on_for_upwind(aorta_image, compare_images, featureType, paramid, write_image):
    name = __name__ + '_test_sigmoid_on_for_gradient_and_upwind_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.SigmoidRemapping = 1
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True


@pytest.mark.parametrize("derivativeValue,paramid", [
    (0.0, '0'),
    (0.5, '1'),
    (4.8, '2'),
])
def test_derivative_sigma_values_for_gradient(aorta_image, compare_images, derivativeValue, paramid, write_image):
    name = __name__ + '_test_derivative_sigma_values_for_gradient_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = 'gradient'
    featurer.DerivativeSigma = derivativeValue
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True


@pytest.mark.parametrize("upwindValue,paramid", [
    (0.0, '0'),
    (0.3, '1'),
])
def test_upwind_factor_values_for_upwind(aorta_image, compare_images, upwindValue, paramid, write_image):
    name = __name__ + '_test_upwind_factor_values_for_upwind_' + paramid + '.mha'
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = 'upwind'
    featurer.UpwindFactor = upwindValue
    featurer.Execute()

    assert compare_images(featurer.Image, name) == True

#TODO: Add test for dimensionality
