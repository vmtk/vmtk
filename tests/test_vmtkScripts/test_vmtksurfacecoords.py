## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
from vmtk import vmtksurfacecoords


def test_coords_arrays_are_added(aorta_surface):
    coords = vmtksurfacecoords.vmtkSurfaceCoords()
    coords.Surface = aorta_surface
    coords.Execute()

    pointData = coords.Surface.GetPointData()
    for name in ('coords', 'coordsX', 'coordsY', 'coordsZ'):
        assert pointData.GetArray(name) is not None


def test_coords_values_match_point_positions(aorta_surface):
    coords = vmtksurfacecoords.vmtkSurfaceCoords()
    coords.Surface = aorta_surface
    coords.Execute()

    surface = coords.Surface
    pointData = surface.GetPointData()
    coordsX = pointData.GetArray('coordsX')
    coordsY = pointData.GetArray('coordsY')
    coordsZ = pointData.GetArray('coordsZ')

    # check a handful of points spread across the surface
    n = surface.GetNumberOfPoints()
    for i in (0, n // 2, n - 1):
        x, y, z = surface.GetPoint(i)
        assert coordsX.GetValue(i) == pytest.approx(x)
        assert coordsY.GetValue(i) == pytest.approx(y)
        assert coordsZ.GetValue(i) == pytest.approx(z)
