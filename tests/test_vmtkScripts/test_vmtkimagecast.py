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
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagecast as cast


@pytest.mark.parametrize("output_type,expected_scalar_size", [
    ("uchar", 1),
    ("short", 2),
    ("float", 4),
    ("double", 8)
])
def test_cast_image_to_types(aorta_image, output_type, expected_scalar_size):

    caster = cast.vmtkImageCast()
    caster.Image = aorta_image
    caster.OutputType = output_type
    caster.Execute()

    assert caster.Image.GetScalarSize() == expected_scalar_size