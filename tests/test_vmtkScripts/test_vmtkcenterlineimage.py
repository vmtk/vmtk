## Program: VMTK
## Language:  Python
## Date:      May 2, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkcenterlineimage as centerlineimage
import sys


@pytest.fixture(scope='module')
def per_system_compare_image_name():
    name = __name__ + '_test_default_parameters_'
    if sys.platform in ['win32', 'win64', 'cygwin']:
        name = name + 'windows.vti'
    elif sys.platform == 'darwin':
        name = name + 'mac.vti'
    else:
        name = name + 'linux.vti'
    return name


def test_default_parameters(aorta_surface, compare_images, per_system_compare_image_name):
    centImage = centerlineimage.vmtkCenterlineImage()
    centImage.Surface = aorta_surface
    centImage.Execute()

    assert compare_images(centImage.Image, per_system_compare_image_name) == True
