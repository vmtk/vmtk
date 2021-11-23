## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
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
import vmtk.vmtkimageshiftscale as imageshiftscale


@pytest.mark.parametrize("outputType,expected_type", [
    ("float", 'float'),
    ("double", 'double'),
    ("uchar", 'unsigned char'),
    ("char", 'char'),
    ("ushort", 'unsigned short'),
    ("short", 'short'),
    ("long", 'long'),
    ("ulong", 'unsigned long'),
    ("int", 'int'),
    ("uint", 'unsigned int'),
])
def test_cast_output_as_types(aorta_image, outputType, expected_type):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.OutputType = outputType
    shifter.Execute()

    assert shifter.Image.GetScalarTypeAsString() == expected_type


def test_shift_value_positive(aorta_image, compare_images):
    name = __name__ + '_test_shift_value_positive.mha'
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Shift = 10.3
    shifter.Execute()

    assert compare_images(shifter.Image, name) == True


def test_shift_value_negative(aorta_image, compare_images):
    name = __name__ + '_test_shift_value_negative.mha'
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Shift = -2.5
    shifter.Execute()

    assert compare_images(shifter.Image, name) == True


def test_scale_value_positive(aorta_image, compare_images):
    name = __name__ + '_test_scale_value_positive.mha'
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Scale = 3.4
    shifter.Execute()

    assert compare_images(shifter.Image, name) == True


def test_scale_value_negative(aorta_image, compare_images):
    name = __name__ + '_test_scale_value_negative.mha'
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Scale = -2.1
    shifter.Execute()

    assert compare_images(shifter.Image, name) == True


@pytest.mark.parametrize("outputRange,paramid", [
    ([0.0, 1.0], '0'),
    ([0.0, 359623], '1'),
    ([-87.3, 35963], '2'),
    ([-124, 0.0], '3'),
    ([-123, -34], '4'),
])
def test_map_ranges(aorta_image, compare_images, outputRange, paramid):
    name = __name__ + '_test_map_ranges_' + paramid + '.mha'
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.MapRanges = 1
    shifter.OutputRange = outputRange
    shifter.Execute()

    assert compare_images(shifter.Image, name) == True
