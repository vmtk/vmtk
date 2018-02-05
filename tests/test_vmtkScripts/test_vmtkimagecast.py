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