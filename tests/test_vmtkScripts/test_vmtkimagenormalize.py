import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagenormalize as norm
import vmtk.vmtkimagereader as r

def test_normalize_image(test_data):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    normer = norm.vmtkImageNormalize()
    normer.Image = reader.Image
    normer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = normer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '0e25a160968487bf9dc9a7217fe9fdb43a96d6f9'