import pytest
import vmtk.vmtkimagefeatures as imagefeatures


#TODO: WHY DOES "fwhm" seg fault?
@pytest.mark.parametrize("featureType,expected_hash", [
    ("vtkgradient", '991e060e29736283055a999fe822da5a87e0cdc0'),
    ("gradient", 'f24ed0b44023295d489efd42594365517fc46598'),
    ("upwind", '58eabd7f68b24fb234fb0bc4468c44e4c4209585'),
])
def test_features_types(aorta_image, image_to_sha, featureType, expected_hash):
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.Execute()

    assert image_to_sha(featurer.Image) == expected_hash


@pytest.mark.parametrize("featureType,expected_hash", [
    ("gradient", '0f53301786c28ead4c84791568e2fd24ea07eb60'),
    ("upwind", '40579c735300420e24aa828d9fd00a549ae96df5'),
])
def test_sigmoid_on_for_gradient_and_upwind(aorta_image, image_to_sha, featureType, expected_hash):
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = featureType
    featurer.SigmoidRemapping = 1
    featurer.Execute()

    assert image_to_sha(featurer.Image) == expected_hash


@pytest.mark.parametrize("derivativeValue,expected_hash", [
    (0.0, 'f24ed0b44023295d489efd42594365517fc46598'),
    (0.5, '12ff807a58037c56045374a8ee5c48c83a4311db'),
    (4.8, '9ad3a9af760597e7e2a7a2f1dbf03c0cefd7e39a'),
])
def test_derivative_sigma_values_for_gradient(aorta_image, image_to_sha, derivativeValue, expected_hash):
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = 'gradient'
    featurer.DerivativeSigma = derivativeValue
    featurer.Execute()

    assert image_to_sha(featurer.Image) == expected_hash


@pytest.mark.parametrize("upwindValue,expected_hash", [
    (0.0, '1fa2a66f430daef46ba16c62730755fe65aedacf'),
    (0.3, '207ca04737f9d09bd2a7051c2db65a96db6c42ea'),
])
def test_upwind_factor_values_for_upwind(aorta_image, image_to_sha, upwindValue, expected_hash):
    featurer = imagefeatures.vmtkImageFeatures()
    featurer.Image = aorta_image
    featurer.FeatureImageType = 'upwind'
    featurer.UpwindFactor = upwindValue
    featurer.Execute()

    assert image_to_sha(featurer.Image) == expected_hash

#TODO: Add test for dimensionality


