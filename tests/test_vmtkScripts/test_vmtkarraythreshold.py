## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
import vtk
from vmtk import vmtkcontribscripts


def _unique_entity_ids(mesh, arrayname='CellEntityIds'):
    arr = mesh.GetCellData().GetArray(arrayname)
    return sorted(set(int(arr.GetComponent(i, 0)) for i in range(arr.GetNumberOfTuples())))


def test_threshold_extracts_only_tetra_volume(aorta_mesh):
    # aorta-mesh.vtu has CellEntityIds {0: tetra volume, 1: triangle surface}
    th = vmtkcontribscripts.vmtkArrayThreshold()
    th.Mesh = aorta_mesh
    th.ArrayName = 'CellEntityIds'
    th.CellData = 1
    th.LowThreshold = 0
    th.HighThreshold = 0
    th.Execute()

    assert th.Mesh.GetNumberOfCells() > 0
    assert th.Mesh.GetNumberOfCells() < aorta_mesh.GetNumberOfCells()
    # only tetrahedra (VTK_TETRA == 10) remain
    types = set(th.Mesh.GetCellType(i) for i in range(th.Mesh.GetNumberOfCells()))
    assert types == {vtk.VTK_TETRA}


def test_threshold_extracts_only_triangle_surface(aorta_mesh):
    th = vmtkcontribscripts.vmtkArrayThreshold()
    th.Mesh = aorta_mesh
    th.ArrayName = 'CellEntityIds'
    th.CellData = 1
    th.LowThreshold = 1
    th.HighThreshold = 1
    th.Execute()

    assert th.Mesh.GetNumberOfCells() > 0
    types = set(th.Mesh.GetCellType(i) for i in range(th.Mesh.GetNumberOfCells()))
    assert types == {vtk.VTK_TRIANGLE}


def test_threshold_partition_is_complete(aorta_mesh):
    total = aorta_mesh.GetNumberOfCells()

    th0 = vmtkcontribscripts.vmtkArrayThreshold()
    th0.Mesh = aorta_mesh
    th0.ArrayName = 'CellEntityIds'
    th0.LowThreshold = 0
    th0.HighThreshold = 0
    th0.Execute()

    th1 = vmtkcontribscripts.vmtkArrayThreshold()
    th1.Mesh = aorta_mesh
    th1.ArrayName = 'CellEntityIds'
    th1.LowThreshold = 1
    th1.HighThreshold = 1
    th1.Execute()

    # the two ids partition every cell of the input mesh
    assert th0.Mesh.GetNumberOfCells() + th1.Mesh.GetNumberOfCells() == total
