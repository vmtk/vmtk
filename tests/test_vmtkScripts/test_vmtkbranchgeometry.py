## Program: VMTK
## Language:  Python
## Date:      February 12, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkbranchgeometry as branchgeometry
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def branch_geometry(aorta_centerline_branches):
    geometry = branchgeometry.vmtkBranchGeometry()
    geometry.Centerlines = aorta_centerline_branches
    geometry.BlankingArrayName = 'Blanking'
    geometry.RadiusArrayName = 'MaximumInscribedSphereRadius'
    geometry.GroupIdsArrayName = 'GroupIds'
    geometry.Execute()

    return geometry.GeometryData


@pytest.mark.parametrize("expectedKey",[
    ('Length'),
    ('Curvature'),
    ('Torsion'),
    ('Tortuosity'),
    ('GroupIds')
])
def test_expected_point_data_keys_present(branch_geometry, expectedKey):
    wrapped_branch_geom = dsa.WrapDataObject(branch_geometry)
    assert expectedKey in wrapped_branch_geom.PointData.keys()


def test_point_data_keys_count_is_5(branch_geometry):
    wrapped_branch_geom = dsa.WrapDataObject(branch_geometry)
    assert len(wrapped_branch_geom.PointData.keys()) == 5


def test_no_cell_data_keys_present(branch_geometry):
    wrapped_branch_geom = dsa.WrapDataObject(branch_geometry)
    assert len(wrapped_branch_geom.CellData.keys()) == 0


def test_branch_points_correct(branch_geometry):
    wrapped_branch_geom = dsa.WrapDataObject(branch_geometry)
    expectedPointLocations = np.array([[0, 0, 0],
                                       [0, 0, 0],
                                       [0, 0, 0]])
    assert np.allclose(wrapped_branch_geom.Points, expectedPointLocations) == True


@pytest.mark.parametrize("pointkey,expectedvalue",[
    ("Length", np.array([35.52495253829437, 35.604787458920114, 34.1129769167953])),
    ("Curvature", np.array([0.01441400600841863, 0.03419512761908704, 0.034160834689055564])),
    ("Torsion", np.array([-0.030372936454314547, -0.17593973521517317, -0.14365517399288139])),
    ("Tortuosity", np.array([0.01062993958363112, 0.01490173952950391, 0.01877139596625832])),
    ("GroupIds", np.array([0.0, 2.0, 3.0]))
])
def test_point_data_is_correct(branch_geometry, pointkey, expectedvalue):
    wrapped_branch_geom = dsa.WrapDataObject(branch_geometry)
    assert np.allclose(wrapped_branch_geom.PointData.GetArray(pointkey), expectedvalue) == True
