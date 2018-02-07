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

def test_read_mha_image(input_datadir, compare_images):
    name = __name__ + '_test_read_mha_image.mha'
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta.mha')
    reader.Execute()

    assert compare_images(reader.Image, name) == True


def test_read_dicom_image(input_datadir, compare_images):
    name = __name__ + '_test_read_dicom_image.dcm'
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'mr-dicom.dcm')
    reader.Execute()

    assert compare_images(reader.Image, name) == True


def test_read_vti_image(input_datadir, compare_images):
    name = __name__ + '_test_read_vti_image.vti'
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'vase.vti')
    reader.Execute()

    assert compare_images(reader.Image, name) == True
