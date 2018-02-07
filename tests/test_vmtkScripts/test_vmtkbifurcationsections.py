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
import vmtk.vmtkbifurcationsections as bifurcationsections
from vtk.numpy_interface import dataset_adapter as dsa 
import numpy as np

@pytest.fixture(scope='module')
def bifur_sections_one_sphere(aorta_centerline_branches ,aorta_surface_branches):
    sections = bifurcationsections.vmtkBifurcationSections()
    sections.Surface = aorta_surface_branches
    sections.Centerlines = aorta_centerline_branches
    sections.NumberOfDistanceSpheres = 1
    sections.RadiusArrayName = 'MaximumInscribedSphereRadius'
    sections.GroupIdsArrayName = 'GroupIds'
    sections.CenterlineIdsArrayName = 'CenterlineIds'
    sections.TractIdsArrayName = 'TractIds'
    sections.BlankingArrayName = 'Blanking'
    sections.Execute()
    
    return sections.BifurcationSections


@pytest.fixture(scope='module')
def bifur_sections_two_spheres(aorta_centerline_branches ,aorta_surface_branches):
    sections = bifurcationsections.vmtkBifurcationSections()
    sections.Surface = aorta_surface_branches
    sections.Centerlines = aorta_centerline_branches
    sections.NumberOfDistanceSpheres = 2
    sections.RadiusArrayName = 'MaximumInscribedSphereRadius'
    sections.GroupIdsArrayName = 'GroupIds'
    sections.CenterlineIdsArrayName = 'CenterlineIds'
    sections.TractIdsArrayName = 'TractIds'
    sections.BlankingArrayName = 'Blanking'
    sections.Execute()
    
    return sections.BifurcationSections


@pytest.mark.parametrize("expectedKey",[
    ('BifurcationSectionGroupIds'),
    ('BifurcationSectionBifurcationGroupIds'),
    ('BifurcationSectionPoint'),
    ('BifurcationSectionNormal'),
    ('BifurcationSectionArea'),
    ('BifurcationSectionMinSize'),
    ('BifurcationSectionMaxSize'),
    ('BifurcationSectionShape'),
    ('BifurcationSectionClosed'),
    ('BifurcationSectionOrientation'),
    ('BifurcationSectionDistanceSpheres')
])
def test_expected_cell_data_keys_present(bifur_sections_one_sphere, expectedKey):
    wrapped_bifur_section = dsa.WrapDataObject(bifur_sections_one_sphere)
    assert expectedKey in wrapped_bifur_section.CellData.keys()


def test_number_expected_cell_data_keys(bifur_sections_one_sphere):
    wrapped_bifur_section = dsa.WrapDataObject(bifur_sections_one_sphere)
    assert len(wrapped_bifur_section.CellData.keys()) == 11


def test_number_expected_point_data_keys(bifur_sections_one_sphere):
    wrapped_bifur_section = dsa.WrapDataObject(bifur_sections_one_sphere)
    assert len(wrapped_bifur_section.PointData.keys()) == 0


def test_expected_number_points_with_one_sphere(bifur_sections_one_sphere):
    wrapped_bifur_section = dsa.WrapDataObject(bifur_sections_one_sphere)
    assert wrapped_bifur_section.Points.shape == (294, 3)


def test_expected_number_points_with_two_spheres(bifur_sections_two_spheres):
    wrapped_bifur_section = dsa.WrapDataObject(bifur_sections_two_spheres)
    assert wrapped_bifur_section.Points.shape == (280, 3)


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([0, 2, 3])),
    (bifur_sections_two_spheres, np.array([0, 2, 3]))
])
def test_bifurcation_section_group_ids_correct(aorta_centerline_branches ,aorta_surface_branches,
                                               bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionGroupIds'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([1, 1, 1])),
    (bifur_sections_two_spheres, np.array([1, 1, 1]))
])
def test_bifurcation_section_bifurcation_group_ids_correct(aorta_centerline_branches ,aorta_surface_branches,
                                                          bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionBifurcationGroupIds'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([[221.44660291, 147.74956102, 23.43517593],
                                          [227.15183902, 130.69595192, 23.69846289],
                                          [216.35230991, 130.63763069, 25.46971588]])),
    (bifur_sections_two_spheres, np.array([[221.36717806, 155.13844109, 22.77446113],
                                           [228.43770649, 126.58995359, 24.12527081],
                                           [215.25035964, 126.36271854, 26.00282479]]))
])
def test_bifurcation_section_point_correct(aorta_centerline_branches ,aorta_surface_branches,
                                           bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionPoint'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([[ 0.01557548, -0.99202661,  0.12506246],
                                          [ 0.4915811,  -0.860392,    0.1344382 ],
                                          [-0.33415118, -0.94115264,  0.05074142]])),
    (bifur_sections_two_spheres, np.array( [[-0.1957447, -0.97325556, 0.12023986],
                                            [ 0.13694085, -0.97859191, 0.15363942],
                                            [-0.15327674, -0.97815097, 0.14045255]]))
])
def test_bifurcation_section_normal_correct(aorta_centerline_branches ,aorta_surface_branches,
                                           bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionNormal'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([177.71780994,   73.02829417,   80.5269763 ])),
    (bifur_sections_two_spheres, np.array([191.24040267,   63.50445732,   64.12766266]))
])
def test_bifurcation_section_area_correct(aorta_centerline_branches ,aorta_surface_branches,
                                          bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionArea'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([14.19050112, 9.18671219, 9.5789813 ])),
    (bifur_sections_two_spheres, np.array([15.12586667, 8.86926931, 8.91067727]))
])
def test_bifurcation_section_min_size_correct(aorta_centerline_branches ,aorta_surface_branches,
                                             bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionMinSize'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([15.9915222, 10.22110037, 10.97796263])),
    (bifur_sections_two_spheres, np.array([16.339976, 9.35342472, 9.27120319]))
])
def test_bifurcation_section_max_size_correct(aorta_centerline_branches ,aorta_surface_branches,
                                             bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionMaxSize'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([ 0.88737651,  0.89879875,  0.87256458])),
    (bifur_sections_two_spheres, np.array([ 0.92569699,  0.94823763,  0.96111336]))
])
def test_bifurcation_section_shape_correct(aorta_centerline_branches ,aorta_surface_branches,
                                           bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionShape'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([1, 1, 0])),
    (bifur_sections_two_spheres, np.array([1, 1, 1]))
])
def test_bifurcation_section_closed_correct(aorta_centerline_branches ,aorta_surface_branches,
                                           bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionClosed'), expectedValue) == True


@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([0, 1, 1])),
    (bifur_sections_two_spheres, np.array([0, 1, 1]))
])
def test_bifurcation_section_orientation_correct(aorta_centerline_branches ,aorta_surface_branches,
                                                 bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionOrientation'), expectedValue) == True

    

@pytest.mark.parametrize("bifurcation_sections,expectedValue",[
    (bifur_sections_one_sphere, np.array([1, 1, 1])),
    (bifur_sections_two_spheres, np.array([2, 2, 2]))
])
def test_bifurcation_section_distance_to_spheres_correct(aorta_centerline_branches ,aorta_surface_branches,
                                                 bifurcation_sections, expectedValue):
    wrapped_bifur_section = dsa.WrapDataObject(bifurcation_sections(aorta_centerline_branches ,aorta_surface_branches)) 
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BifurcationSectionDistanceSpheres'), expectedValue) == True


@pytest.mark.parametrize("expectedvalue,paramid", [
    (124, 0),
    (80, 1),
    (90, 2)
])
def test_number_of_points_per_cell(bifur_sections_one_sphere, expectedvalue, paramid):
    bcx = bifur_sections_one_sphere.GetCell(paramid)
    numberOfPoints = bcx.GetNumberOfPoints()

    assert numberOfPoints == expectedvalue
    

@pytest.mark.parametrize("expectedvalue,paramid", [
    (0, 0),
    (124, 1),
    (204, 2)
])
def test_cell_data_pointId_start_indices(bifur_sections_one_sphere, expectedvalue, paramid):
    bcx = bifur_sections_one_sphere.GetCell(paramid)
    pointIdStart = bcx.GetPointId(0)

    assert pointIdStart == expectedvalue


@pytest.mark.parametrize("expectedvalue,numberofpoints,paramid", [
    (123, 124, 0),
    (203, 80, 1),
    (293, 90, 2)
])
def test_cell_data_pointId_end_indices(bifur_sections_one_sphere, expectedvalue, numberofpoints, paramid):
    bcx = bifur_sections_one_sphere.GetCell(paramid)
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)

    assert pointIdEnd == expectedvalue


@pytest.mark.parametrize("pointidstart,numberofpoints,expectedlocationstart,expectedlocationend,paramid", [
    (0, 124, np.array([ 220.9937439,   146.86668396,   16.48843384]), np.array([ 221.39068604,  146.87068176,   16.47060013]), 0),
    (124, 80, np.array([ 225.22320557,  128.92987061,   19.44786835]), np.array([ 225.25431824,  128.94456482,   19.42810059]), 1),
    (204, 90, np.array([ 221.40382385,  128.67816162,   22.39156914]), np.array([ 221.80430603,  128.64775085,   24.46484756]), 2)
])
def test_cell_data_point_start_and_end_xyz_locations(bifur_sections_one_sphere, pointidstart, numberofpoints, 
                                                     expectedlocationstart, expectedlocationend, paramid):
    bcx = bifur_sections_one_sphere.GetCell(paramid)
    bw = dsa.WrapDataObject(bifur_sections_one_sphere)
    
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)
    pointLocationEnd = bw.Points[pointIdEnd]
    pointLocationStart = bw.Points[pointidstart]

    assert np.allclose(np.array(pointLocationStart), expectedlocationstart) == True
    assert np.allclose(np.array(pointLocationEnd), expectedlocationend) == True