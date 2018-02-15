## Program: VMTK
## Language:  Python
## Date:      February 7, 2018
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
import vmtk.vmtkbifurcationvectors as bifurcationvectors
import vmtk.vmtksurfacereader as surfacereader
import os
from vtk.numpy_interface import dataset_adapter as dsa 
import numpy as np


@pytest.fixture(scope='module')
def reference_system(input_datadir):
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-centerline-referencesystem.vtp')
    reader.Execute()
    return reader.Surface

@pytest.fixture(scope='module')
def bifurcation_vectors(reference_system, aorta_centerline_branches):
    vectors = bifurcationvectors.vmtkBifurcationVectors()
    vectors.Centerlines = aorta_centerline_branches
    vectors.ReferenceSystems = reference_system
    vectors.RadiusArrayName = "MaximumInscribedSphereRadius"
    vectors.GroupIdsArrayName = "GroupIds"
    vectors.CenterlineIdsArrayName = "CenterlineIds"
    vectors.TractIdsArrayName = "TractIds"
    vectors.BlankingArrayName = "Blanking"
    vectors.ReferenceSystemsNormalArrayName = "Normal"
    vectors.ReferenceSystemsUpNormalArrayName = "UpNormal"
    vectors.NormalizeBifurcationVectors = 0
    vectors.Execute()

    return vectors.BifurcationVectors


def test_number_of_points_is_three(bifurcation_vectors):
    assert bifurcation_vectors.GetNumberOfPoints() == 3


def test_number_of_point_arrays_is_eight(bifurcation_vectors):
    assert bifurcation_vectors.GetPointData().GetNumberOfArrays() == 8


@pytest.mark.parametrize("arraynum,expectedname", [
    (0, "BifurcationVectors"),
    (1, "InPlaneBifurcationVectors"),
    (2, "OutOfPlaneBifurcationVectors"),
    (3, "InPlaneBifurcationVectorAngles"),
    (4, "OutOfPlaneBifurcationVectorAngles"),
    (5, "BifurcationVectorsOrientation"),
    (6, "GroupIds"),
    (7, "BifurcationGroupIds"),
])
def test_point_data_arrays_names_align_with_expected(bifurcation_vectors, arraynum, expectedname):
    pointData = bifurcation_vectors.GetPointData()
    arrayname = pointData.GetArrayName(arraynum)
    assert arrayname == expectedname


@pytest.mark.parametrize("arraynum,expectednum", [
    (0, 3),
    (1, 3),
    (2, 3),
    (3, 1),
    (4, 1),
    (5, 1),
    (6, 1),
    (7, 1),
])
def test_point_data_arrays_number_of_components_align_with_expected(bifurcation_vectors, arraynum, expectednum):
    pointData = bifurcation_vectors.GetPointData()
    numberofcomponents = pointData.GetArray(arraynum).GetNumberOfComponents()
    assert numberofcomponents == expectednum


def test_point_locations_are_where_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([[ 221.4466095 ,  147.7495575 ,   23.43517685],
                               [ 225.06373596,  134.70544434,   23.55709076],
                               [ 218.53140259,  134.76171875,   24.90955925]])

    assert np.allclose(wrapped.Points, expectedPoints) == True


def test_bifurcation_vector_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([[ 0.24890407, -6.95878637,  0.45230165],
                               [ 2.08810306, -4.00949241,  0.14137213],
                               [-2.17909268, -4.12408806,  0.56015663]])

    assert np.allclose(wrapped.PointData['BifurcationVectors'], expectedPoints) == True


def test_inplane_bifurcation_vector_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([[ 0.23626338, -6.96323822,  0.37824108],
                               [ 2.04566927, -4.02443691, -0.10724324],
                               [-2.16923032, -4.1206147 ,  0.61793922]])

    assert np.allclose(wrapped.PointData['InPlaneBifurcationVectors'], expectedPoints) == True


def test_outofplane_bifurcation_vector_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([[ 0.01264069,  0.00445185,  0.07406057],
                               [ 0.04243379,  0.0149445 ,  0.24861537],
                               [-0.00986236, -0.00347336, -0.05778259]])

    assert np.allclose(wrapped.PointData['OutOfPlaneBifurcationVectors'], expectedPoints) == True


def test_inplane_bifurcation_vector_angle_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([ 0.03805914,  0.48117113, -0.48381398])

    assert np.allclose(wrapped.PointData['InPlaneBifurcationVectorAngles'], expectedPoints) == True


def test_outofplane_bifurcation_vector_angle_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([-0.01078616, -0.05589054,  0.01249973])

    assert np.allclose(wrapped.PointData['OutOfPlaneBifurcationVectorAngles'], expectedPoints) == True


def test_bifurcation_vector_orientation_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([0, 1, 1])

    assert np.allclose(wrapped.PointData['BifurcationVectorsOrientation'], expectedPoints) == True


def test_groupid_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([0, 2, 3])

    assert np.allclose(wrapped.PointData['GroupIds'], expectedPoints) == True


def test_bifurcation_groupid_values_align_with_expected(bifurcation_vectors):
    wrapped = dsa.WrapDataObject(bifurcation_vectors)
    expectedPoints = np.array([1, 1, 1])

    assert np.allclose(wrapped.PointData['BifurcationGroupIds'], expectedPoints) == True