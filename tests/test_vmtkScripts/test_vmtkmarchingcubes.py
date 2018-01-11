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
import vmtk.vmtkmarchingcubes as marchingcubes
import numpy as np

@pytest.fixture()
def level_set_image(test_data):
    import vmtk.vmtkimagereader as reader
    import os
    read = reader.vmtkImageReader()
    read.InputFileName = os.path.join(test_data, 'aorta-final-levelset.mha')
    read.Execute()

    return read.Image


def test_marching_cubes_default(aorta_surface, level_set_image, poly_to_np):
    mc = marchingcubes.vmtkMarchingCubes()
    mc.Image = level_set_image
    mc.Level = 0.0
    mc.Execute()

    referencesurf = poly_to_np(aorta_surface)
    comparesurf = poly_to_np(mc.Surface)

    assert np.allclose(referencesurf['Points'], comparesurf['Points'])
    assert np.allclose(referencesurf['CellData']['CellPointIds'], comparesurf['CellData']['CellPointIds'])

