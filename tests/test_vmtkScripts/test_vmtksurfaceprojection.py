## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
import vtk
from vmtk import vmtksurfaceprojection


@pytest.fixture(scope='module')
def reference_with_arrays(aorta_surface):
    # build an INDEPENDENT reference surface (a deep copy, so it is not the
    # same object as the input) carrying a known point-data and cell-data array
    surface = vtk.vtkPolyData()
    surface.DeepCopy(aorta_surface)

    pointArray = vtk.vtkDoubleArray()
    pointArray.SetName('RefPointArray')
    pointArray.SetNumberOfComponents(1)
    pointArray.SetNumberOfTuples(surface.GetNumberOfPoints())
    pointArray.FillComponent(0, 3.0)
    surface.GetPointData().AddArray(pointArray)

    cellArray = vtk.vtkDoubleArray()
    cellArray.SetName('RefCellArray')
    cellArray.SetNumberOfComponents(1)
    cellArray.SetNumberOfTuples(surface.GetNumberOfCells())
    cellArray.FillComponent(0, 7.0)
    surface.GetCellData().AddArray(cellArray)
    return surface


def test_linear_projection_copies_point_array(aorta_surface, reference_with_arrays):
    proj = vmtksurfaceprojection.vmtkSurfaceProjection()
    proj.Surface = aorta_surface
    proj.ReferenceSurface = reference_with_arrays
    proj.Method = 'linear'
    proj.Execute()

    assert proj.Surface.GetPointData().GetArray('RefPointArray') is not None


def test_closestpoint_projection_copies_point_and_cell_arrays(aorta_surface, reference_with_arrays):
    proj = vmtksurfaceprojection.vmtkSurfaceProjection()
    proj.Surface = aorta_surface
    proj.ReferenceSurface = reference_with_arrays
    proj.Method = 'closestpoint'
    proj.Execute()

    out = proj.Surface
    assert out.GetPointData().GetArray('RefPointArray') is not None
    projectedCell = out.GetCellData().GetArray('RefCellArray')
    assert projectedCell is not None
    # projecting a constant reference cell array preserves that constant
    for i in range(projectedCell.GetNumberOfTuples()):
        assert projectedCell.GetValue(i) == pytest.approx(7.0)
