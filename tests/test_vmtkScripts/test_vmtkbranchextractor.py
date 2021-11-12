## Program: VMTK
## Language:  Python
## Date:      February 2, 2018
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
import vmtk.vmtkbranchextractor as branchextractor
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def centerline_branches(aorta_centerline):
    extractor = branchextractor.vmtkBranchExtractor()
    extractor.Centerlines = aorta_centerline
    extractor.RadiusArrayName = 'MaximumInscribedSphereRadius'
    extractor.Execute()

    return extractor.Centerlines


def test_centerline_surface_distance(centerline_branches, compare_centerlines):
    name = __name__ + '_test_centerline_surface_distance.vtp'
    assert compare_centerlines(centerline_branches, name) == True


def test_number_of_lines(centerline_branches):
    assert centerline_branches.GetNumberOfLines() == 6


def test_number_of_cells(centerline_branches):
    assert centerline_branches.GetNumberOfCells() == 6


def test_number_of_points(centerline_branches):
    assert centerline_branches.GetNumberOfPoints() == 417


def test_cell_data_array_names(centerline_branches):
    expected_array_names = [
        "CenterlineIds",
        "TractIds",
        "Blanking",
        "GroupIds",
    ]
    cell_data = centerline_branches.GetCellData()
    array_names = [cell_data.GetArrayName(idx) for idx in range(cell_data.GetNumberOfArrays())]
    assert sorted(expected_array_names) == sorted(array_names)


@pytest.mark.parametrize("expectedvalue,paramname", [
    ([0, 0, 0, 1, 1, 1], "CenterlineIds"),
    ([0, 1, 2, 0, 1, 2], "TractIds"),
    ([0, 1, 0, 0, 1, 0], "Blanking"),
    ([0, 1, 2, 0, 1, 3], "GroupIds")
])
def test_cell_data_array_values(centerline_branches, expectedvalue, paramname):
    centerline_branches_wrapped = dsa.WrapDataObject(centerline_branches)
    assert centerline_branches_wrapped.CellData.GetArray(paramname).tolist() == expectedvalue


@pytest.mark.parametrize("expectedvalue,paramid", [
    (78, 0),
    (21, 1),
    (116, 2),
    (73, 3),
    (26, 4),
    (103, 5)
])
def test_number_of_points_per_cell(centerline_branches, expectedvalue, paramid):
    bcx = centerline_branches.GetCell(paramid)
    numberOfPoints = bcx.GetNumberOfPoints()

    assert numberOfPoints == expectedvalue


@pytest.mark.parametrize("expectedvalue,paramid", [
    (0, 0),
    (78, 1),
    (99, 2),
    (215, 3),
    (288, 4),
    (314, 5)
])
def test_cell_data_pointId_start_indices(centerline_branches, expectedvalue, paramid):
    bcx = centerline_branches.GetCell(paramid)
    pointIdStart = bcx.GetPointId(0)

    assert pointIdStart == expectedvalue


@pytest.mark.parametrize("expectedvalue,numberofpoints,paramid", [
    (77, 78, 0),
    (98, 21, 1),
    (214, 116, 2),
    (287, 73, 3),
    (313, 26, 4),
    (416, 103, 5)
])
def test_cell_data_pointId_end_indices(centerline_branches, expectedvalue, numberofpoints, paramid):
    bcx = centerline_branches.GetCell(paramid)
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)

    assert pointIdEnd == expectedvalue


@pytest.mark.parametrize("pointidstart,numberofpoints,expectedlocationstart,expectedlocationend,paramid", [
    (0, 78, np.array([ 222.09629822, 175.8699646, 21.67310715]), np.array([ 222.55067444, 140.65298462, 23.77128601]), 0),
    (77, 21, np.array([222.55067444, 140.65298462, 23.77128601]), np.array([225.06373596, 134.70544434, 23.55709076]), 1),
    (99, 116, np.array([225.06373596, 134.70544434, 23.55709076]), np.array([234.35397339, 101.31446838, 28.98675537]), 2),
    (215, 73, np.array([222.09629822, 175.8699646, 21.67310715]), np.array([220.89038086, 140.92050171, 23.99687004]), 3),
    (288, 26, np.array([220.89038086, 140.92050171, 23.99687004]), np.array([218.53140259, 134.76171875, 24.90955925]), 4),
    (314, 103, np.array([218.53140259, 134.76171875, 24.90955925]), np.array([210.20037842, 103.04645538, 31.68768501]), 5)
])
def test_cell_data_point_start_and_end_xyz_locations(centerline_branches, pointidstart, numberofpoints,
                                                     expectedlocationstart, expectedlocationend, paramid):
    bcx = centerline_branches.GetCell(paramid)
    bw = dsa.WrapDataObject(centerline_branches)

    pointIdEnd = bcx.GetPointId(numberofpoints - 1)
    pointLocationEnd = bw.Points[pointIdEnd]
    pointLocationStart = bw.Points[pointidstart]

    assert np.allclose(np.array(pointLocationStart), expectedlocationstart) == True
    assert np.allclose(np.array(pointLocationEnd), expectedlocationend) == True
