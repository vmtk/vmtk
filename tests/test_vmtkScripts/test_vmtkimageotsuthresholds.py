import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimageotsuthresholds as otsu
import vmtk.vmtkimagereader as r

@pytest.mark.parametrize("thresholds,expected_hash", [
    (1, 'd692f244d88a2f4544fd111d26a337048568b75e'),
    (2, '4f3320f66ca6d50e53ef7da620bb3cc2c90e6069'),
    (3, '46381069b0def5db65f51a27568d0a6ed19ee790'),
])
def test_otsu_image_multiple_thresholds(aorta_image, thresholds, expected_hash):
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfThresholds = thresholds
    otsuer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = otsuer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == expected_hash


def test_otsu_image_256_histogram_bins(aorta_image):

    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfHistogramBins = 256
    otsuer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = otsuer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '2666f107392cbc193d3c9cd1572637e2a1d17f6c'