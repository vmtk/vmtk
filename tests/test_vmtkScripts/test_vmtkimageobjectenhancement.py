import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimageobjectenhancement as enhance

def test_enhance_image_with_defaults(aorta_image):
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = enhancer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '016b31cec594351a073c725bd5195d1fad0542e3'

def test_enhance_image_with_scaled_objectiveness(aorta_image):
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.ScaledObjectness = True
    enhancer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = enhancer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '016b31cec594351a073c725bd5195d1fad0542e3'


@pytest.mark.parametrize("sigma_min,sigma_max,expected_hash", [
    (0.5, 1.0, 'b26636b9c637f5fb77feadbb848dc4798cd3fae6'),
    (1.0, 2.0, '016b31cec594351a073c725bd5195d1fad0542e3'),
    (2.0, 1.0, '9080c6f281475ab6fd87b9d407c9d5c7adceb171'),
    (0.7, 2.3, '54ac334b5e045ce8a96f693839a94fce9ed754e7')
])
def test_enhance_image_with_changing_sigma(aorta_image, sigma_min, sigma_max, expected_hash):
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.SigmaMin = sigma_min
    enhancer.SigmaMax = sigma_max
    enhancer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = enhancer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == expected_hash


@pytest.mark.parametrize("alpha,beta,gamma,expected_hash", [
    (0.5, 1.0, 5.0, '016b31cec594351a073c725bd5195d1fad0542e3'),
    (0.5, 0.5, 10.0, '243c0b9c432ff345df77b3e5af58437815aae996'),
    (2.0, 0.5, 10.0, 'e02ea9a2a76a909442cb1fbc4c163aff04dfd292'),
])
def test_enhance_image_with_changing_abg(aorta_image, alpha, beta, gamma, expected_hash):
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.Alpha = alpha
    enhancer.Beta = beta
    enhancer.Gamma = gamma
    enhancer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = enhancer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == expected_hash


def test_enhance_image_three_sigma_steps(aorta_image):
    enhancer = enhance.vmtkImageObjectEnhancement()
    enhancer.Image = aorta_image
    enhancer.NumberOfSigmaSteps = 3
    enhancer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = enhancer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == 'a54565857cb3318515dacd1083884838aa693661'
