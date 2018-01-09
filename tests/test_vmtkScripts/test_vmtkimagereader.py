import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap

def test_read_mha_image(test_data):
    import vmtk.vmtkimagereader as r
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = reader.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '08c6d50899ecfbe5ad42d9c17703b54f2b4fe428'


def test_read_dicom_image(test_data):
    import vmtk.vmtkimagereader as r
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'mr-dicom.dcm')
    reader.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = reader.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '2ceb5680a790e14b8f67efc02d4e63e05bb77bad'


def test_read_vti_image(test_data):
    import vmtk.vmtkimagereader as r
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'vase.vti')
    reader.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = reader.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['SLCImage'].copy(order='C')

    assert sha1(check).hexdigest() == '1a7bd4ca031bccd322bea06ff1fa6d69e45c63db'