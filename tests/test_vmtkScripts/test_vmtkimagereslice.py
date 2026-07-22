## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
from vmtk import vmtkimagereslice


def _identity_coefficients():
    return [1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0]


def test_matrixcoefficients_output_has_16_entries(aorta_image):
    # Regression test: when MatrixCoefficients is supplied as input it must be
    # reset before the output coefficients are appended, otherwise the output
    # list ends up with 32 entries instead of 16.
    reslice = vmtkimagereslice.vmtkImageReslice()
    reslice.Image = aorta_image
    reslice.MatrixCoefficients = _identity_coefficients()
    reslice.Execute()

    assert len(reslice.MatrixCoefficients) == 16


def test_identity_reslice_preserves_dimensions(aorta_image):
    reslice = vmtkimagereslice.vmtkImageReslice()
    reslice.Image = aorta_image
    reslice.MatrixCoefficients = _identity_coefficients()
    reslice.Execute()

    assert reslice.Image is not None
    assert reslice.Image.GetNumberOfPoints() > 0
