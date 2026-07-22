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


def test_extract_volume_entity_from_mesh(aorta_mesh):
    # extract id 0 (tetra volume) from aorta-mesh.vtu {0: tetra, 1: triangle}
    extractor = vmtkcontribscripts.vmtkEntityExtractor()
    extractor.Mesh = aorta_mesh
    extractor.CellEntityIdsArrayName = 'CellEntityIds'
    extractor.EntityIds = [0]
    extractor.Execute()

    assert extractor.Mesh.GetNumberOfCells() > 0
    types = set(extractor.Mesh.GetCellType(i) for i in range(extractor.Mesh.GetNumberOfCells()))
    assert types == {vtk.VTK_TETRA}
    # the deleted part holds the complementary (triangle) cells
    assert extractor.DeletedMesh.GetNumberOfCells() > 0
    deletedTypes = set(extractor.DeletedMesh.GetCellType(i) for i in range(extractor.DeletedMesh.GetNumberOfCells()))
    assert deletedTypes == {vtk.VTK_TRIANGLE}


def test_extract_and_deleted_partition_the_mesh(aorta_mesh):
    extractor = vmtkcontribscripts.vmtkEntityExtractor()
    extractor.Mesh = aorta_mesh
    extractor.CellEntityIdsArrayName = 'CellEntityIds'
    extractor.EntityIds = [0]
    extractor.Execute()

    total = extractor.Mesh.GetNumberOfCells() + extractor.DeletedMesh.GetNumberOfCells()
    assert total == aorta_mesh.GetNumberOfCells()


def test_invert_selects_complement(aorta_mesh):
    extractor = vmtkcontribscripts.vmtkEntityExtractor()
    extractor.Mesh = aorta_mesh
    extractor.CellEntityIdsArrayName = 'CellEntityIds'
    extractor.EntityIds = [0]
    extractor.Invert = 1
    extractor.Execute()

    # inverting id 0 keeps the triangle surface (id 1)
    assert extractor.OutputEntityIds == [1]
    types = set(extractor.Mesh.GetCellType(i) for i in range(extractor.Mesh.GetNumberOfCells()))
    assert types == {vtk.VTK_TRIANGLE}


def test_single_id_as_scalar_is_accepted(aorta_mesh):
    # regression: a single id passed as an int (not a list) must not raise
    extractor = vmtkcontribscripts.vmtkEntityExtractor()
    extractor.Mesh = aorta_mesh
    extractor.CellEntityIdsArrayName = 'CellEntityIds'
    extractor.EntityIds = 0
    extractor.Execute()

    assert extractor.Mesh.GetNumberOfCells() > 0
