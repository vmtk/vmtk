## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
import vtk
from vmtk import vmtksurfacetransform


def _identity_coefficients():
    return [1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0]


def test_translation_moves_points(aorta_surface):
    transformer = vmtksurfacetransform.vmtkSurfaceTransform()
    transformer.Surface = aorta_surface
    transformer.Translation = [10.0, 0.0, 0.0]
    transformer.Execute()

    inBounds = aorta_surface.GetBounds()
    outBounds = transformer.Surface.GetBounds()
    # x bounds shifted by +10, y and z unchanged
    assert outBounds[0] == pytest.approx(inBounds[0] + 10.0, abs=1e-6)
    assert outBounds[1] == pytest.approx(inBounds[1] + 10.0, abs=1e-6)
    assert outBounds[2] == pytest.approx(inBounds[2], abs=1e-6)


def test_matrixcoefficients_output_has_16_entries(aorta_surface):
    # Regression test: MatrixCoefficients is declared both as an input and an
    # output member. When supplied as input it must not be appended to again,
    # otherwise the output list ends up with 32 entries instead of 16.
    transformer = vmtksurfacetransform.vmtkSurfaceTransform()
    transformer.Surface = aorta_surface
    transformer.MatrixCoefficients = _identity_coefficients()
    transformer.Execute()

    assert len(transformer.MatrixCoefficients) == 16


def test_identity_matrixcoefficients_is_a_noop(aorta_surface):
    transformer = vmtksurfacetransform.vmtkSurfaceTransform()
    transformer.Surface = aorta_surface
    transformer.MatrixCoefficients = _identity_coefficients()
    transformer.Execute()

    inBounds = aorta_surface.GetBounds()
    outBounds = transformer.Surface.GetBounds()
    for i in range(6):
        assert outBounds[i] == pytest.approx(inBounds[i], abs=1e-6)


def test_newzdirection_produces_16_coefficients(aorta_surface):
    transformer = vmtksurfacetransform.vmtkSurfaceTransform()
    transformer.Surface = aorta_surface
    transformer.NewZDirection = [1.0, 0.0, 0.0]
    transformer.Execute()

    assert len(transformer.MatrixCoefficients) == 16
    assert transformer.Surface.GetNumberOfPoints() == aorta_surface.GetNumberOfPoints()
