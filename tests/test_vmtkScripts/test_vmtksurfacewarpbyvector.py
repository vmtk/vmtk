## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
import vtk
from vmtk import vmtksurfacewarpbyvector
from vmtk import vmtksurfacenormals


@pytest.fixture(scope='module')
def surface_with_normals(aorta_surface):
    normals = vmtksurfacenormals.vmtkSurfaceNormals()
    normals.Surface = aorta_surface
    normals.NormalsArrayName = 'Normals'
    normals.Execute()
    return normals.Surface


def test_zero_scale_leaves_points_unchanged(surface_with_normals):
    warp = vmtksurfacewarpbyvector.vmtkSurfaceWarpByVector()
    warp.Surface = surface_with_normals
    warp.WarpArrayName = 'Normals'
    warp.ScaleFactor = 0.0
    warp.Execute()

    n = warp.Surface.GetNumberOfPoints()
    assert n == surface_with_normals.GetNumberOfPoints()
    for i in (0, n // 2, n - 1):
        assert warp.Surface.GetPoint(i) == pytest.approx(surface_with_normals.GetPoint(i))


def test_unit_scale_moves_points_by_the_vector(surface_with_normals):
    scale = 1.5
    reference = vtk.vtkPolyData()
    reference.DeepCopy(surface_with_normals)

    warp = vmtksurfacewarpbyvector.vmtkSurfaceWarpByVector()
    warp.Surface = surface_with_normals
    warp.WarpArrayName = 'Normals'
    warp.ScaleFactor = scale
    warp.Execute()

    normals = reference.GetPointData().GetArray('Normals')
    for i in (0, reference.GetNumberOfPoints() // 2, reference.GetNumberOfPoints() - 1):
        original = reference.GetPoint(i)
        vector = normals.GetTuple3(i)
        expected = [original[j] + scale * vector[j] for j in range(3)]
        assert warp.Surface.GetPoint(i) == pytest.approx(expected, abs=1e-5)
