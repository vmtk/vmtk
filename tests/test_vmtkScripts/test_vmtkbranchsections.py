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
import vmtk.vmtkbranchsections as branchsections
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np


@pytest.fixture(scope='module')
def branch_sections_one_sphere(aorta_centerline_branches ,aorta_surface_branches):
    sections = branchsections.vmtkBranchSections()
    sections.Surface = aorta_surface_branches
    sections.Centerlines = aorta_centerline_branches
    sections.NumberOfDistanceSpheres = 1
    sections.ReverseDirection = 0
    sections.RadiusArrayName = 'MaximumInscribedSphereRadius'
    sections.GroupIdsArrayName = 'GroupIds'
    sections.CenterlineIdsArrayName = 'CenterlineIds'
    sections.TractIdsArrayName = 'TractIds'
    sections.BlankingArrayName = 'Blanking'
    sections.Execute()

    return sections.BranchSections


@pytest.fixture(scope='module')
def branch_sections_two_spheres(aorta_centerline_branches ,aorta_surface_branches):
    sections = branchsections.vmtkBranchSections()
    sections.Surface = aorta_surface_branches
    sections.Centerlines = aorta_centerline_branches
    sections.NumberOfDistanceSpheres = 2
    sections.ReverseDirection = 0
    sections.RadiusArrayName = 'MaximumInscribedSphereRadius'
    sections.GroupIdsArrayName = 'GroupIds'
    sections.CenterlineIdsArrayName = 'CenterlineIds'
    sections.TractIdsArrayName = 'TractIds'
    sections.BlankingArrayName = 'Blanking'
    sections.Execute()

    return sections.BranchSections


def test_number_of_cells_one_sphere(branch_sections_one_sphere):
    numberOfCells = branch_sections_one_sphere.GetNumberOfCells()

    assert numberOfCells == 24


def test_number_of_cells_two_sphere(branch_sections_two_spheres):
    numberOfCells = branch_sections_two_spheres.GetNumberOfCells()

    assert numberOfCells == 13


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3])),
    ("branch_sections_two_spheres", np.array([0, 0, 0, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3]))
])
def test_branch_section_group_ids_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionGroupIds'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([ 195.53117732, 182.23256278, 186.21267186, 187.0400059,  177.20153242,
                                            176.25756012,  85.82336158,  73.02829417,  63.50445732,  62.40968092,
                                            62.22208797,  60.62570948,  60.78477703,  60.01402702,  63.08210028,
                                            99.06819265,  80.5269763,   64.12766266,  64.57327767,  67.13289619,
                                            60.67602206,  59.98268905,  58.48300609,  58.6038296 ])),
    ("branch_sections_two_spheres", np.array([ 195.53117732, 186.21267186, 177.20153242,  85.82336158,  63.50445732,
                                             62.22208797,  60.78477703,  63.08210028,  99.06819265,  64.12766266,
                                             67.13289619,  59.98268905,  58.6038296 ]))
])
def test_branch_section_area_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionArea'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([ 15.25387687, 14.25260369, 14.66768231, 15.25974257, 14.6356421,
                                            13.64498788, 10.89010723,  9.18671219,  8.86926931,  8.74859368,
                                            8.56866816,  8.61375309,  8.58205574,  8.49087216,  8.73891524,
                                            11.33372646,  9.5789813,   8.91067727,  8.55769926,  8.87761983,
                                            8.63328033,   8.53398992,   8.28912586,   8.73934951])),
    ("branch_sections_two_spheres", np.array([ 15.25387687, 14.66768231, 14.6356421,  10.89010723,  8.86926931,
                                            8.56866816,  8.58205574,  8.73891524, 11.33372646,  8.91067727,
                                            8.87761983,  8.53398992,  8.73934951]))
])
def test_branch_section_min_size_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionMinSize'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([ 17.08821628, 16.06283909, 16.22629607, 15.95819134, 16.01361226,
                                            16.17715589, 11.69644525, 10.22110037,  9.35342472,  9.36595157,
                                            9.21275981,  9.20696121,  9.04795408,  9.16998689,  9.37937275,
                                            12.45697059, 10.97796263,  9.27120319,  9.39964383,  9.83837421,
                                            9.22775579,  9.13391134,  8.9179931,   8.86614888])),
    ("branch_sections_two_spheres", np.array([ 17.08821628, 16.22629607, 16.01361226, 11.69644525,  9.35342472,
                                            9.21275981,  9.04795408,  9.37937275, 12.45697059,  9.27120319,
                                            9.83837421,  9.13391134,  8.86614888]))
])
def test_branch_section_max_size_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionMaxSize'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([ 0.89265472, 0.8873029,  0.90394519, 0.95623259, 0.91395007, 0.84347261,
                                            0.93106127, 0.89879875, 0.94823763, 0.93408487, 0.930087,   0.93556961,
                                            0.94850788, 0.92594158, 0.93171638, 0.90983007, 0.87256458, 0.96111336,
                                            0.91042804, 0.90234622, 0.93557746, 0.93431933, 0.92948332, 0.98569848])),
    ("branch_sections_two_spheres", np.array([ 0.89265472, 0.90394519, 0.91395007, 0.93106127, 0.94823763, 0.930087,
                                            0.94850788, 0.93171638, 0.90983007, 0.96111336, 0.90234622, 0.93431933,
                                            0.98569848]))
])
def test_branch_section_shape_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionShape'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1])),
    ("branch_sections_two_spheres", np.array([1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1]))
])
def test_branch_section_closed_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionClosed'), expectedValue) == True


@pytest.mark.parametrize("branch_sections,expectedValue",[
    ("branch_sections_one_sphere", np.array([0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8])),
    ("branch_sections_two_spheres", np.array([0, 2, 4, 0, 2, 4, 6, 8, 0, 2, 4, 6, 8]))
])
def test_branch_section_distance_to_spheres_correct(branch_sections, expectedValue, request):
    branch_sections = request.getfixturevalue(branch_sections)
    wrapped_bifur_section = dsa.WrapDataObject(branch_sections)
    assert np.allclose(wrapped_bifur_section.CellData.GetArray('BranchSectionDistanceSpheres'), expectedValue) == True


@pytest.mark.parametrize("expectedvalue,paramid",[
    (0, 0),
    (129, 1),
    (246, 2),
    (369, 3),
    (491, 4),
    (610, 5),
    (712, 6),
    (784, 7),
    (864, 8),
    (938, 9),
    (1012, 10),
    (1084, 11),
    (1157, 12),
    (1227, 13),
    (1294, 14),
    (1362, 15),
    (1440, 16),
    (1530, 17),
    (1604, 18),
    (1685, 19),
    (1760, 20),
    (1838, 21),
    (1916, 22),
    (1986, 23)
])
def test_cell_data_pointId_start_indices_one_sphere(branch_sections_one_sphere, expectedvalue, paramid):
    bcx = branch_sections_one_sphere.GetCell(paramid)
    pointIdStart = bcx.GetPointId(0)

    assert pointIdStart == expectedvalue


@pytest.mark.parametrize("expectedvalue,paramid",[
    (0, 0),
    (129, 1),
    (252, 2),
    (371, 3),
    (443, 4),
    (517, 5),
    (589, 6),
    (659, 7),
    (727, 8),
    (805, 9),
    (879, 10),
    (954, 11),
    (1032, 12),
])
def test_cell_data_pointId_start_indices_two_sphere(branch_sections_two_spheres, expectedvalue, paramid):
    bcx = branch_sections_two_spheres.GetCell(paramid)
    pointIdStart = bcx.GetPointId(0)

    assert pointIdStart == expectedvalue


@pytest.mark.parametrize("expectedvalue,numberofpoints,paramid",[
    (128, 129, 0),
    (245, 117, 1),
    (368, 123, 2),
    (490, 122, 3),
    (609, 119, 4),
    (711, 102, 5),
    (783, 72, 6),
    (863, 80, 7),
    (937, 74, 8),
    (1011, 74, 9),
    (1083, 72, 10),
    (1156, 73, 11),
    (1226, 70, 12),
    (1293, 67, 13),
    (1361, 68, 14),
    (1439, 78, 15),
    (1529, 90, 16),
    (1603, 74, 17),
    (1684, 81, 18),
    (1759, 75, 19),
    (1837, 78, 20),
    (1915, 78, 21),
    (1985, 70, 22),
    (2064, 79, 23)
])
def test_cell_data_pointId_end_indices_one_sphere(branch_sections_one_sphere, expectedvalue, numberofpoints, paramid):
    bcx = branch_sections_one_sphere.GetCell(paramid)
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)

    assert pointIdEnd == expectedvalue


@pytest.mark.parametrize("expectedvalue,numberofpoints,paramid",[
    (128, 129, 0),
    (251, 123, 1),
    (370, 119, 2),
    (442, 72, 3),
    (516, 74, 4),
    (588, 72, 5),
    (658, 70, 6),
    (726, 68, 7),
    (804, 78, 8),
    (878, 74, 9),
    (953, 75, 10),
    (1031, 78, 11),
    (1110, 79, 12),
])
def test_cell_data_pointId_end_indices_two_spheres(branch_sections_two_spheres, expectedvalue, numberofpoints, paramid):
    bcx = branch_sections_two_spheres.GetCell(paramid)
    pointIdEnd = bcx.GetPointId(numberofpoints - 1)

    assert pointIdEnd == expectedvalue


@pytest.mark.parametrize("pointidstart,numberofpoints,expectedlocationstart,expectedlocationend,paramid",[
    (0, 129, np.array([217.96841430664062, 173.62118530273438, 13.255617141723633]), np.array([218.3564910888672, 173.5325927734375, 13.078214645385742]), 0),
    (129, 117, np.array([220.60513305664062, 168.85765075683594, 14.689851760864258]), np.array([220.7154998779297, 168.8603515625, 14.662433624267578]), 1),
    (246, 123, np.array([220.60513305664062, 161.35403442382812, 14.91808795928955]), np.array([221.1270294189453, 161.36172485351562, 14.869494438171387]), 2),
    (369, 122, np.array([217.96841430664062, 153.799560546875, 16.24483871459961]), np.array([218.7977752685547, 153.759033203125, 15.950116157531738]), 3),
    (491, 119, np.array([220.60513305664062, 147.16163635253906, 16.457529067993164]), np.array([220.84371948242188, 147.17149353027344, 16.445602416992188]), 4),
    (610, 102, np.array([214.2646026611328, 138.3168182373047, 20.781129837036133]), np.array([215.2498321533203, 139.0978546142578, 28.59796714782715]), 5),
    (712, 72, np.array([221.03619384765625, 133.0021209716797, 19.41550064086914]), np.array([223.09197998046875, 133.60549926757812, 29.31968116760254]), 6),
    (784, 80, np.array([225.22320556640625, 128.92987060546875, 19.44786834716797]), np.array([225.2543182373047, 128.94456481933594, 19.4281005859375]), 7),
    (864, 74, np.array([224.99966430664062, 125.61617279052734, 20.987215042114258]), np.array([225.03799438476562, 125.61742401123047, 20.96105194091797]), 8),
    (938, 74, np.array([226.9380340576172, 121.55354309082031, 20.983095169067383]), np.array([227.32139587402344, 121.60404968261719, 20.776121139526367]), 9),
    (1012, 72, np.array([230.27310180664062, 117.59968566894531, 20.985858917236328]), np.array([230.3741455078125, 117.61669921875, 20.980459213256836]), 10),
    (1084, 73, np.array([229.39419555664062, 112.8595962524414, 22.42799949645996]), np.array([229.47718811035156, 112.87579345703125, 22.390670776367188]), 11),
    (1157, 70, np.array([230.27310180664062, 108.89535522460938, 23.66469383239746]), np.array([230.82315063476562, 108.984375, 23.356685638427734]), 12),
    (1227, 67, np.array([232.90982055664062, 105.00150299072266, 23.84995460510254]), np.array([232.92904663085938, 105.00645446777344, 23.845577239990234]), 13),
    (1294, 68, np.array([232.12461853027344, 101.83521270751953, 25.46320915222168]), np.array([232.1511993408203, 101.83283996582031, 25.44672393798828]), 14),
    (1362, 78, np.array([221.04122924804688, 132.8127899169922, 19.44769287109375]), np.array([223.05599975585938, 133.10491943359375, 29.166749954223633]), 15),
    (1440, 90, np.array([221.40382385253906, 128.67816162109375, 22.391569137573242]), np.array([221.80430603027344, 128.6477508544922, 24.464847564697266]), 16),
    (1530, 74, np.array([216.21060180664062, 125.59019470214844, 21.670682907104492]), np.array([216.93014526367188, 125.52423095703125, 21.99653434753418]), 17),
    (1604, 81, np.array([214.45278930664062, 121.28471374511719, 22.488676071166992]), np.array([214.6112518310547, 121.27181243896484, 22.50135040283203]), 18),
    (1685, 75, np.array([212.6949920654297, 117.73933410644531, 23.282371520996094]), np.array([213.42185974121094, 117.62139892578125, 23.06829071044922]), 19),
    (1760, 78, np.array([213.5738983154297, 113.08839416503906, 23.80427360534668]), np.array([214.01332092285156, 112.93942260742188, 23.97063446044922]), 20),
    (1838, 78, np.array([210.9371795654297, 108.91300201416016, 24.94631004333496]), np.array([211.59010314941406, 108.7583999633789, 24.869142532348633]), 21),
    (1916, 70, np.array([211.8160858154297, 104.5389404296875, 25.843137741088867]), np.array([212.57681274414062, 104.47166442871094, 26.245433807373047]), 22),
    (1986, 79, np.array([210.0582733154297, 101.82815551757812, 26.50916290283203]), np.array([210.67491149902344, 101.69086456298828, 26.563392639160156]), 23)
])
def test_cell_data_point_start_and_end_xyz_locations_one_sphere(branch_sections_one_sphere, pointidstart, numberofpoints,
                                                                expectedlocationstart, expectedlocationend, paramid):
    bcx = branch_sections_one_sphere.GetCell(paramid)
    bw = dsa.WrapDataObject(branch_sections_one_sphere)

    pointIdEnd = bcx.GetPointId(numberofpoints - 1)
    pointLocationEnd = bw.Points[pointIdEnd]
    pointLocationStart = bw.Points[pointidstart]

    assert np.allclose(np.array(pointLocationStart), expectedlocationstart) == True
    assert np.allclose(np.array(pointLocationEnd), expectedlocationend) == True


@pytest.mark.parametrize("pointidstart,numberofpoints,expectedlocationstart,expectedlocationend,paramid",[
    (0, 129, np.array([217.96841430664062, 173.62118530273438, 13.255617141723633]), np.array([218.3564910888672, 173.5325927734375, 13.078214645385742]), 0),
    (129, 123, np.array([220.60513305664062, 161.35403442382812, 14.91808795928955]), np.array([221.1270294189453, 161.36172485351562, 14.869494438171387]), 1),
    (252, 119, np.array([220.60513305664062, 147.16163635253906, 16.457529067993164]), np.array([220.84371948242188, 147.17149353027344, 16.445602416992188]), 2),
    (371, 72, np.array([221.03619384765625, 133.0021209716797, 19.41550064086914]), np.array([223.09197998046875, 133.60549926757812, 29.31968116760254]), 3),
    (443, 74, np.array([224.99966430664062, 125.61617279052734, 20.987215042114258]), np.array([225.03799438476562, 125.61742401123047, 20.96105194091797]), 4),
    (517, 72, np.array([230.27310180664062, 117.59968566894531, 20.985858917236328]), np.array([230.3741455078125, 117.61669921875, 20.980459213256836]), 5),
    (589, 70, np.array([230.27310180664062, 108.89535522460938, 23.66469383239746]), np.array([230.82315063476562, 108.984375, 23.356685638427734]), 6),
    (659, 68, np.array([232.12461853027344, 101.83521270751953, 25.46320915222168]), np.array([232.1511993408203, 101.83283996582031, 25.44672393798828]), 7),
    (727, 78, np.array([221.04122924804688, 132.8127899169922, 19.44769287109375]), np.array([223.05599975585938, 133.10491943359375, 29.166749954223633]), 8),
    (805, 74, np.array([216.21060180664062, 125.59019470214844, 21.670682907104492]), np.array([216.93014526367188, 125.52423095703125, 21.99653434753418]), 9),
    (879, 75, np.array([212.6949920654297, 117.73933410644531, 23.282371520996094]), np.array([213.42185974121094, 117.62139892578125, 23.06829071044922]), 10),
    (954, 78, np.array([210.9371795654297, 108.91300201416016, 24.94631004333496]), np.array([211.59010314941406, 108.7583999633789, 24.869142532348633]), 11),
    (1032, 79, np.array([210.0582733154297, 101.82815551757812, 26.50916290283203]), np.array([210.67491149902344, 101.69086456298828, 26.563392639160156]), 12),
])
def test_cell_data_point_start_and_end_xyz_locations_two_spheres(branch_sections_two_spheres, pointidstart, numberofpoints,
                                                                expectedlocationstart, expectedlocationend, paramid):
    bcx = branch_sections_two_spheres.GetCell(paramid)
    bw = dsa.WrapDataObject(branch_sections_two_spheres)

    pointIdEnd = bcx.GetPointId(numberofpoints - 1)
    pointLocationEnd = bw.Points[pointIdEnd]
    pointLocationStart = bw.Points[pointidstart]

    assert np.allclose(np.array(pointLocationStart), expectedlocationstart) == True
    assert np.allclose(np.array(pointLocationEnd), expectedlocationend) == True
