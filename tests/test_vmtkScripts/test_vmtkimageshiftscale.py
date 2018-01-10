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


def test_shift_value_positive(aorta_image, image_to_sha):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Shift = 10.3
    shifter.Execute()

    assert image_to_sha(shifter.Image) == '9fa33d56e51117263ff1c58e7189dea26928207c'


def test_shift_value_negative(aorta_image, image_to_sha):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Shift = -2.5
    shifter.Execute()
    
    assert image_to_sha(shifter.Image) == '03c91bc849923c87066b5de1b2f1c348b4e0d5d1'


def test_scale_value_positive(aorta_image, image_to_sha):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Scale = 3.4
    shifter.Execute()
    
    assert image_to_sha(shifter.Image) == '5679e1f03693757866bcaf1e3ed8b6110ce1ce36'


def test_scale_value_negative(aorta_image, image_to_sha):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.Scale = -2.1
    shifter.Execute()
    
    assert image_to_sha(shifter.Image) == '6d52fbbf9c43b401c6f7e521dee794b088208aaa'


@pytest.mark.parametrize("outputRange,expected_hash", [
    ([0.0, 1.0], 'bd734150c34102355e22054693c3576c9c4a48b1'),
    ([0.0, 359623], 'a0224ce03741db4699c8fe28885be234fb31574e'),
    ([-87.3, 35963], '2bc06d8726fbe873217281d95f7bc188eb53ac8c'),
    ([-124, 0.0], 'd45ea1214bbbf5aaf2f79fccc4b811e23fecac32'),
    ([-123, -34], '040bb6992fa330d7af810c74aaf783091c886b2d'),
])
def test_map_ranges(aorta_image, image_to_sha, outputRange, expected_hash):
    shifter = imageshiftscale.vmtkImageShiftScale()
    shifter.Image = aorta_image
    shifter.MapRanges = 1
    shifter.OutputRange = outputRange
    shifter.Execute()

    assert image_to_sha(shifter.Image) == expected_hash