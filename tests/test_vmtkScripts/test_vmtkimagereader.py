## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import os
import vmtk.vmtkimagereader as imagereader

def test_read_mha_image(input_datadir, image_to_sha):
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta.mha')
    reader.Execute()

    assert image_to_sha(reader.Image) == '08c6d50899ecfbe5ad42d9c17703b54f2b4fe428'


def test_read_dicom_image(input_datadir, image_to_sha):
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'mr-dicom.dcm')
    reader.Execute()

    assert image_to_sha(reader.Image) == '2ceb5680a790e14b8f67efc02d4e63e05bb77bad'


def test_read_vti_image(input_datadir):
    import vmtk.vmtkimagetonumpy as imagetonumpy
    from hashlib import sha1

    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'vase.vti')
    reader.Execute()

    # need to do a manual conversion since the array point data key is non
    # standard for this test data object
    converter = imagetonumpy.vmtkImageToNumpy()
    converter.Image = reader.Image
    converter.Execute()
    check = converter.ArrayDict['PointData']['SLCImage'].copy(order='C')

    assert sha1(check).hexdigest() == '1a7bd4ca031bccd322bea06ff1fa6d69e45c63db'