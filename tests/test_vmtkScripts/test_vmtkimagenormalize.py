import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagenormalize as norm

def test_normalize_image(aorta_image):
    normer = norm.vmtkImageNormalize()
    normer.Image = aorta_image
    normer.Execute()

    conv = wrap.vmtkImageToNumpy()
    conv.Image = normer.Image
    conv.Execute()

    check = conv.ArrayDict['PointData']['ImageScalars'].copy(order='C')

    assert sha1(check).hexdigest() == '0e25a160968487bf9dc9a7217fe9fdb43a96d6f9'