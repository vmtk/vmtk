## Program: VMTK
## Language:  Python
## Date:      February 12, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtkbifurcationprofiles as bifurcationprofiles
from vtk.numpy_interface import dataset_adapter as dsa 
import numpy as np

@pytest.fixture(scope='module')
def bifur_profiles(aorta_centerline_branches ,aorta_surface_branches):
    profiles = bifurcationprofiles.vmtkBifurcationProfiles()
    profiles.Surface = aorta_surface_branches
    profiles.Centerlines = aorta_centerline_branches
    profiles.RadiusArrayName = 'MaximumInscribedSphereRadius'
    profiles.GroupIdsArrayName = 'GroupIds'
    profiles.CenterlineIdsArrayName = 'CenterlineIds'
    profiles.TractIdsArrayName = 'TractIds'
    profiles.BlankingArrayName = 'Blanking'
    profiles.Execute()

    return profiles.BifurcationProfiles


@pytest.mark.parametrize("expectedKey",[
    ('BifurcationProfileGroupIds'),
    ('BifurcationProfileBifurcationGroupIds'),
    ('BifurcationProfileOrientation')
 ])
def test_expected_cell_data_keys_present(bifur_profiles, expectedKey):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    assert expectedKey in wrapped_bifur_profile.CellData.keys()


def test_number_expected_cell_data_keys(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    assert len(wrapped_bifur_profile.CellData.keys()) == 3


def test_number_expected_point_data_keys(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    assert len(wrapped_bifur_profile.PointData.keys()) == 0


def test_expected_number_points(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    assert wrapped_bifur_profile.Points.shape == (358, 3)


def test_bifurcation_profile_group_ids_correct(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    expectedValue = np.array([0, 2, 3])
    assert np.allclose(wrapped_bifur_profile.CellData.GetArray('BifurcationProfileGroupIds'), expectedValue) == True


def test_bifurcation_profile_bifurcation_group_ids_correct(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    expectedValue = np.array([1, 1, 1])
    assert np.allclose(wrapped_bifur_profile.CellData.GetArray('BifurcationProfileBifurcationGroupIds'), expectedValue) == True


def test_bifurcation_profile_orientation_correct(bifur_profiles):
    wrapped_bifur_profile = dsa.WrapDataObject(bifur_profiles)
    expectedValue = np.array([0, 1, 1])
    assert np.allclose(wrapped_bifur_profile.CellData.GetArray('BifurcationProfileOrientation'), expectedValue) == True


def test_number_of_lines(bifur_profiles):
    assert bifur_profiles.GetNumberOfLines() == 3


def test_number_of_cells(bifur_profiles):
    assert bifur_profiles.GetNumberOfCells() == 3


def test_number_of_points(bifur_profiles):
    assert bifur_profiles.GetNumberOfPoints() == 358


@pytest.mark.parametrize("expectedvalue,paramid", [
    (132, 0),
    (106, 1),
    (120, 2)
])
def test_number_of_points_per_cell(bifur_profiles, expectedvalue, paramid):
    bcx = bifur_profiles.GetCell(paramid)
    numberOfPoints = bcx.GetNumberOfPoints()

    assert numberOfPoints == expectedvalue
    

@pytest.mark.parametrize("expectedvalue,paramid", [
    (0, 0),
    (132, 1),
    (238, 2)
])
def test_cell_data_pointId_start_indices(bifur_profiles, expectedvalue, paramid):
    bcx = bifur_profiles.GetCell(paramid)
    pointIdStart = bcx.GetPointId(0)

    assert pointIdStart == expectedvalue


@pytest.mark.parametrize("expectedvalue,numberofpoints,paramid", [
    (131, 132, 0),
    (237, 106, 1),
    (357, 120, 2)
])
def test_cell_data_pointId_end_indices(bifur_profiles, expectedvalue, numberofpoints, paramid):
    bcx = bifur_profiles.GetCell(paramid)
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)

    assert pointIdEnd == expectedvalue


@pytest.mark.parametrize("pointidstart,numberofpoints,expectedlocationstart,expectedlocationend,paramid", [
    (0, 132, np.array([ 221.89123535, 135.91229248,  17.98954964]), np.array([ 221.82603455, 135.88433838,  18.00107956]), 0),
    (132, 106, np.array([ 222.36294556, 136.11395264,  17.92448616]), np.array([ 222.40290833, 136.13102722,  17.91927338]), 1),
    (238, 120, np.array([ 221.05075073, 132.63262939,  19.5011692 ]), np.array([ 221.05207825, 132.61680603,  19.50825119]), 2)
])
def test_cell_data_point_start_and_end_xyz_locations(bifur_profiles, pointidstart, numberofpoints, 
                                                     expectedlocationstart, expectedlocationend, paramid):
    bcx = bifur_profiles.GetCell(paramid)
    bw = dsa.WrapDataObject(bifur_profiles)
    
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)
    pointLocationEnd = bw.Points[pointIdEnd]
    pointLocationStart = bw.Points[pointidstart]

    assert np.allclose(np.array(pointLocationStart), expectedlocationstart) == True
    assert np.allclose(np.array(pointLocationEnd), expectedlocationend) == True