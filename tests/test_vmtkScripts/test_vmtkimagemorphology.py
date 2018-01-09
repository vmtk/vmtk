import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagemorphology as morph
import vmtk.vmtkimagereader as r

def test_dilate_grayscale_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    morpher = morph.vmtkImageMorphology()
    morpher.Image = reader.Image
    morpher.Operation = 'dilate'
    morpher.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = morpher.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '489fc2a5f372426e6c2cd4d29a95c81383e9bbcc'


def test_erode_grayscale_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    morpher = morph.vmtkImageMorphology()
    morpher.Image = reader.Image
    morpher.Operation = 'erode'
    morpher.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = morpher.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == 'eacee45a6e32c1cef043644cafbd5f823c3ef5d9'


def test_open_grayscale_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    morpher = morph.vmtkImageMorphology()
    morpher.Image = reader.Image
    morpher.Operation = 'open'
    morpher.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = morpher.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '83419dc74e3785f4c9689ef5d0c404fca035fd2b'


def test_close_grayscale_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    morpher = morph.vmtkImageMorphology()
    morpher.Image = reader.Image
    morpher.Operation = 'close'
    morpher.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = morpher.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '56c558031c5a5251dd2aa5d8d1503736d68252f5'


def test_change_ball_radius_dilate_grayscale_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    morpher = morph.vmtkImageMorphology()
    morpher.Image = reader.Image
    morpher.Operation = 'dilate'
    morpher.BallRadius = [2, 2, 2]
    morpher.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = morpher.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == 'e2be7af55aec94a544380804771bc2b9feca75b0'