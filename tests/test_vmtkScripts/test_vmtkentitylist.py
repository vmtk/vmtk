## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
from vmtk import vmtkcontribscripts


def test_entitylist_on_mesh(aorta_mesh):
    # aorta-mesh.vtu has CellEntityIds {0: tetra volume, 1: triangle surface}
    lister = vmtkcontribscripts.vmtkEntityList()
    lister.Mesh = aorta_mesh
    lister.CellEntityIdsArrayName = 'CellEntityIds'
    lister.Execute()

    assert lister.EntityIds == [0, 1]
    assert lister.VolumeEntityIds == [0]
    assert lister.SurfaceEntityIds == [1]
    assert lister.LineEntityIds == []


def test_entitylist_ids_are_python_ints(aorta_mesh):
    lister = vmtkcontribscripts.vmtkEntityList()
    lister.Mesh = aorta_mesh
    lister.CellEntityIdsArrayName = 'CellEntityIds'
    lister.Execute()

    for i in lister.EntityIds:
        assert isinstance(i, int)
