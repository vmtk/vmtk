## Program: VMTK
## Language:  Python
## Date:      July 18, 2026
## Version:   1.5

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
import vtk
import numpy as np
from vtk.util import numpy_support
import vmtk.vmtkmeshaddexternallayer as meshaddexternallayer


@pytest.fixture()
def aorta_mesh_copy(aorta_mesh):
    # vmtkMeshAddExternalLayer offsets the CellEntityIds of its input mesh in
    # place, so hand each test its own copy to keep the shared module-scope
    # fixture pristine
    mesh = vtk.vtkUnstructuredGrid()
    mesh.DeepCopy(aorta_mesh)
    return mesh


def test_add_external_layer(aorta_mesh_copy, compare_meshes):
    inputCells = aorta_mesh_copy.GetNumberOfCells()

    adder = meshaddexternallayer.vmtkMeshAddExternalLayer()
    adder.Mesh = aorta_mesh_copy
    adder.Execute()

    ids = numpy_support.vtk_to_numpy(adder.Mesh.GetCellData().GetArray('CellEntityIds'))
    assert adder.Mesh.GetNumberOfCells() > inputCells
    assert np.issubdtype(ids.dtype, np.integer)
    # input ids are {0: volume, 1: boundary surface}; the boundary is remapped
    # to 2 and the external layer adds its volume cells (id 1) and the outer
    # extruded surface (id 3)
    assert set(ids.tolist()) == {0, 1, 2, 3}
    assert set(ids[inputCells:].tolist()) == {1, 3}
    assert compare_meshes(adder.Mesh, 'aorta-mesh-external-layer.vtu',
                          method='cellarray', arrayname='CellEntityIds')


def test_add_external_layer_open_profiles(aorta_mesh_copy):
    # relabel the boundary cells above the mid-plane as inlet/outlet caps so
    # that extruding the remaining wall produces open profiles
    ids = numpy_support.vtk_to_numpy(aorta_mesh_copy.GetCellData().GetArray('CellEntityIds'))
    cellCenters = vtk.vtkCellCenters()
    cellCenters.SetInputData(aorta_mesh_copy)
    cellCenters.Update()
    z = numpy_support.vtk_to_numpy(cellCenters.GetOutput().GetPoints().GetData())[:, 2]
    zMid = 0.5 * (aorta_mesh_copy.GetBounds()[4] + aorta_mesh_copy.GetBounds()[5])
    ids[(ids == 1) & (z > zMid)] = 2
    aorta_mesh_copy.GetCellData().GetArray('CellEntityIds').Modified()

    adder = meshaddexternallayer.vmtkMeshAddExternalLayer()
    adder.Mesh = aorta_mesh_copy
    adder.Execute()

    outIds = set(numpy_support.vtk_to_numpy(adder.Mesh.GetCellData().GetArray('CellEntityIds')).tolist())
    # original ids: volume 0->0, wall 1->2, caps 2->4; external layer: volume
    # cells 1, outer extruded surface 3, extruded open profiles from 5 upwards
    assert {0, 1, 2, 3, 4, 5}.issubset(outIds)
