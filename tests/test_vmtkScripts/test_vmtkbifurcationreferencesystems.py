## Program: VMTK
## Language:  Python
## Date:      February 7, 2018
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
import vmtk.vmtkbifurcationreferencesystems as bifurcationreferencesystems
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def bifurcation_reference_system(aorta_centerline_branches):
    bifur = bifurcationreferencesystems.vmtkBifurcationReferenceSystems()
    bifur.Centerlines = aorta_centerline_branches
    bifur.RadiusArrayName = 'MaximumInscribedSphereRadius'
    bifur.BlankingArrayName = 'Blanking'
    bifur.GroupIdsArrayName = 'GroupIds'
    bifur.Execute()

    return bifur.ReferenceSystems


def test_reference_system_composed_of_3_arrays(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    assert dataArrays.GetNumberOfArrays() == 3


def test_array_0_name_is_normal(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    assert dataArrays.GetArrayName(0) == 'Normal'


def test_array_0_has_three_components(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray(0)
    assert array.GetNumberOfComponents() == 3


def test_array_1_name_is_upnormal(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    assert dataArrays.GetArrayName(1) == 'UpNormal'


def test_array_1_has_three_components(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray(1)
    assert array.GetNumberOfComponents() == 3


def test_array_2_name_is_groupids(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    assert dataArrays.GetArrayName(2) == 'GroupIds'


def test_array_2_has_one_components(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray(2)
    assert array.GetNumberOfComponents() == 1


def test_reference_system_contains_one_points(bifurcation_reference_system):
    assert bifurcation_reference_system.GetNumberOfPoints() == 1


def test_bifurcation_point_at_expected_location(bifurcation_reference_system):
    expectedLocation = np.array([221.66326904296875, 138.3250732421875, 24.043575286865234])
    assert np.allclose(np.array(bifurcation_reference_system.GetPoint(0)), expectedLocation) == True


def test_normal_array_values_match_expected(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray('Normal')
    computedValue = np.array([array.GetComponent(0, 0), array.GetComponent(0, 1), array.GetComponent(0, 2)])
    expectedValue = np.array([-0.16795282, -0.05915027, -0.98401885])

    assert np.allclose(computedValue, expectedValue) == True


def test_upnormal_array_values_match_expected(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray('UpNormal')
    computedValue = np.array([array.GetComponent(0, 0), array.GetComponent(0, 1), array.GetComponent(0, 2)])
    expectedValue = np.array([-0.00365111, -0.99815405,  0.06062313])

    assert np.allclose(computedValue, expectedValue) == True


def test_groupids_array_values_match_expected(bifurcation_reference_system):
    dataArrays = bifurcation_reference_system.GetPointData()
    array = dataArrays.GetArray('GroupIds')
    computedValue = np.array([array.GetComponent(0, 0)])
    expectedValue = np.array([1.0])

    assert np.allclose(computedValue, expectedValue) == True
