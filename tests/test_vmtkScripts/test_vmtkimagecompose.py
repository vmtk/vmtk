import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagecompose as comp

def test_multiply_images(aorta_image):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = composer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '9b87984045e3756840562fe06fbf88e63be3c7d3'

def test_subtract_images(aorta_image):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'subtract'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = composer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '2d717c12ea94d12d75c2b2412661cbb1e193c5e2'

def test_negate_image2_and_multiply(aorta_image):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = composer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '002b8f78b7da8375bc31d762edcba8867bcb79bf'


def test_negate_image2_and_min(aorta_image):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'min'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = composer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == 'be29a3a239cc798c3de812921f2d3a9c82b9cd2f'


def test_negate_image2_and_max(aorta_image):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'max'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = composer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == 'd84290a3d556f5eb7c5e25a8075c9078791ec57d'