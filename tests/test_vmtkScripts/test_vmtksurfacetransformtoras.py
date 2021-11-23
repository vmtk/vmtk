## Program: VMTK
## Language:  Python
## Date:      April 9, 2018
## Version:   1.4.1

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import os
import vmtk.vmtksurfacetransformtoras as transformtoras


@pytest.fixture(scope='module')
def transform_image(input_datadir):
    import vmtk.vmtkimagereader as imagereader
    reader = imagereader.vmtkImageReader()
    reader.InputFileName = os.path.join(input_datadir, 'image-test-transform.nrrd')
    reader.Execute()
    return (reader.Image, reader.XyzToRasMatrixCoefficients)


def test_default_parameters(transform_image, compare_surfaces):
    name = __name__ + '_test_default_parameters.stl'
    import vmtk.vmtkmarchingcubes as marching
    image = transform_image[0]
    matrixCoefficients = transform_image[1]
    mc = marching.vmtkMarchingCubes()
    mc.Image = image
    mc.Level = 3000.0
    mc.Execute()

    transform = transformtoras.vmtkSurfaceTransformToRAS()
    transform.Surface = mc.Surface
    transform.XyzToRasMatrixCoefficients = matrixCoefficients
    transform.Execute()
    assert compare_surfaces(transform.Surface, name) == True


def test_invert_on(transform_image, compare_surfaces):
    name = __name__ + '_test_invert_on.stl'
    import vmtk.vmtkmarchingcubes as marching
    image = transform_image[0]
    matrixCoefficients = transform_image[1]
    mc = marching.vmtkMarchingCubes()
    mc.Image = image
    mc.Level = 3000.0
    mc.Execute()

    transform = transformtoras.vmtkSurfaceTransformToRAS()
    transform.Surface = mc.Surface
    transform.XyzToRasMatrixCoefficients = matrixCoefficients
    transform.InvertMatrix = 1
    transform.Execute()
    assert compare_surfaces(transform.Surface, name) == True
