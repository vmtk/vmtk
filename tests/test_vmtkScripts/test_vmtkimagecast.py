import pytest
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtkimagecast as cast
import vmtk.vmtkimagereader as r


@pytest.mark.parametrize("output_type,expected_scalar_size", [
    ("uchar", 1),
    ("short", 2),
    ("float", 4),
    ("double", 8)
])
def test_cast_image_to_types(test_data, output_type, expected_scalar_size):
    reader = r.vmtkImageReader()
    reader.InputFileName = os.path.join(test_data, 'aorta.mha')
    reader.Execute()

    caster = cast.vmtkImageCast()
    caster.Image = reader.Image
    caster.OutputType = output_type
    caster.Execute()

    assert caster.Image.GetScalarSize() == expected_scalar_size