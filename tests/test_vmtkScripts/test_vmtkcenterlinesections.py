## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import math

import pytest
import vtk

import vmtk.vmtkcenterlinesections as centerlinesections


TUBE_LENGTH = 6.0
TUBE_RADIUS = 1.0
TUBE_RESOLUTION = 24
# Area of the regular polygon that a plane perpendicular to the tube axis cuts from the tube
SECTION_AREA = 0.5 * TUBE_RESOLUTION * TUBE_RADIUS ** 2 * math.sin(2.0 * math.pi / TUBE_RESOLUTION)

ARRAY_NAMES = ['CenterlineSectionArea', 'CenterlineSectionMinSize', 'CenterlineSectionMaxSize',
               'CenterlineSectionShape', 'CenterlineSectionClosed']


def transformed(polyData, rotationAngle):
    transform = vtk.vtkTransform()
    transform.RotateX(rotationAngle)
    transformFilter = vtk.vtkTransformPolyDataFilter()
    transformFilter.SetInputData(polyData)
    transformFilter.SetTransform(transform)
    transformFilter.Update()
    return transformFilter.GetOutput()


def tube(rotationAngle=0.0):
    '''An open-ended circular tube along z, rotated about the x axis by rotationAngle degrees.
    It has a ring of points every 0.5 along its axis.'''
    numberOfRings = 13
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfRings):
        z = TUBE_LENGTH * i / (numberOfRings - 1.0)
        for j in range(TUBE_RESOLUTION):
            angle = 2.0 * math.pi * (j + 0.5) / TUBE_RESOLUTION
            points.InsertNextPoint(TUBE_RADIUS * math.cos(angle), TUBE_RADIUS * math.sin(angle), z)
    for i in range(numberOfRings - 1):
        for j in range(TUBE_RESOLUTION):
            p0 = i * TUBE_RESOLUTION + j
            p1 = i * TUBE_RESOLUTION + (j + 1) % TUBE_RESOLUTION
            polys.InsertNextCell(4, [p0, p1, p1 + TUBE_RESOLUTION, p0 + TUBE_RESOLUTION])
    surface = vtk.vtkPolyData()
    surface.SetPoints(points)
    surface.SetPolys(polys)
    return transformed(surface, rotationAngle)


def centerlines(zCoordinatesPerLine, rotationAngle=0.0):
    '''Centerlines along the axis of tube(rotationAngle), one polyline for each list of z coordinates.'''
    points = vtk.vtkPoints()
    lines = vtk.vtkCellArray()
    for zCoordinates in zCoordinatesPerLine:
        lines.InsertNextCell(len(zCoordinates))
        for z in zCoordinates:
            lines.InsertCellPoint(points.InsertNextPoint(0.0, 0.0, z))
    polyData = vtk.vtkPolyData()
    polyData.SetPoints(points)
    polyData.SetLines(lines)
    return transformed(polyData, rotationAngle)


def compute_sections(surface, centerlines):
    sections = centerlinesections.vmtkCenterlineSections()
    sections.Surface = surface
    sections.Centerlines = centerlines
    sections.Execute()
    return sections


# Between the rings of tube points, so that the sections cut edges rather than points
Z_BETWEEN_RINGS = [0.75 + 0.5 * i for i in range(10)]


@pytest.mark.parametrize('rotationAngle', [0.0, 45.0])
def test_sections_along_straight_tube(rotationAngle):
    result = compute_sections(tube(rotationAngle), centerlines([Z_BETWEEN_RINGS], rotationAngle))
    sections = result.CenterlineSections

    assert sections.GetNumberOfCells() == len(Z_BETWEEN_RINGS)
    for i in range(sections.GetNumberOfCells()):
        assert sections.GetCell(i).GetNumberOfPoints() == TUBE_RESOLUTION
    cellData = sections.GetCellData()
    for i in range(sections.GetNumberOfCells()):
        assert cellData.GetArray('CenterlineSectionArea').GetValue(i) == pytest.approx(SECTION_AREA, rel=1e-6)
        assert cellData.GetArray('CenterlineSectionClosed').GetValue(i) == 1
        minSize = cellData.GetArray('CenterlineSectionMinSize').GetValue(i)
        maxSize = cellData.GetArray('CenterlineSectionMaxSize').GetValue(i)
        assert 0.0 < minSize <= maxSize
        assert cellData.GetArray('CenterlineSectionShape').GetValue(i) == pytest.approx(minSize / maxSize)


def test_centerline_point_data_matches_sections():
    result = compute_sections(tube(), centerlines([Z_BETWEEN_RINGS[:4], Z_BETWEEN_RINGS[4:]]))
    sections = result.CenterlineSections
    centerlinePointData = result.Centerlines.GetPointData()

    assert sections.GetNumberOfCells() == len(Z_BETWEEN_RINGS)
    for name in ARRAY_NAMES:
        sectionArray = sections.GetCellData().GetArray(name)
        centerlineArray = centerlinePointData.GetArray(name)
        assert sectionArray is not None
        assert centerlineArray is not None
        assert centerlineArray.GetNumberOfTuples() == result.Centerlines.GetNumberOfPoints()
        # One section per centerline point, in the order of the centerline cells and their points
        for i in range(sections.GetNumberOfCells()):
            assert centerlineArray.GetTuple1(i) == sectionArray.GetTuple1(i)


def test_section_is_perpendicular_to_centerline():
    # Along a centerline that bends, each section is cut perpendicular to the average of the
    # directions to the neighboring centerline points
    surface = tube()
    points = vtk.vtkPoints()
    for point in [(0.0, -0.2, 2.25), (0.0, 0.0, 2.75), (0.0, 0.2, 3.25)]:
        points.InsertNextPoint(point)
    lines = vtk.vtkCellArray()
    lines.InsertNextCell(3, [0, 1, 2])
    polyline = vtk.vtkPolyData()
    polyline.SetPoints(points)
    polyline.SetLines(lines)

    sections = compute_sections(surface, polyline).CenterlineSections

    assert sections.GetNumberOfCells() == 3
    expectedNormals = []
    for direction in [(0.0, 0.2, 0.5), (0.0, 0.4, 1.0), (0.0, 0.2, 0.5)]:
        normal = list(direction)
        vtk.vtkMath.Normalize(normal)
        expectedNormals.append(normal)
    for i in range(3):
        cell = sections.GetCell(i)
        origin = points.GetPoint(i)
        for k in range(cell.GetNumberOfPoints()):
            sectionPoint = sections.GetPoint(cell.GetPointId(k))
            offset = [sectionPoint[j] - origin[j] for j in range(3)]
            assert vtk.vtkMath.Dot(offset, expectedNormals[i]) == pytest.approx(0.0, abs=1e-6)


def test_custom_array_names():
    sections = centerlinesections.vmtkCenterlineSections()
    sections.Surface = tube()
    sections.Centerlines = centerlines([Z_BETWEEN_RINGS])
    sections.CenterlineSectionAreaArrayName = 'Area'
    sections.CenterlineSectionMinSizeArrayName = 'MinSize'
    sections.CenterlineSectionMaxSizeArrayName = 'MaxSize'
    sections.CenterlineSectionShapeArrayName = 'Shape'
    sections.CenterlineSectionClosedArrayName = 'Closed'
    sections.Execute()

    for name in ['Area', 'MinSize', 'MaxSize', 'Shape', 'Closed']:
        assert sections.CenterlineSections.GetCellData().GetArray(name) is not None
        assert sections.Centerlines.GetPointData().GetArray(name) is not None


def test_duplicated_end_point():
    # Centerlines often end with the same point twice. Both copies get a section perpendicular to
    # the centerline instead of a plane with an undefined normal.
    zCoordinates = Z_BETWEEN_RINGS + [Z_BETWEEN_RINGS[-1]]
    result = compute_sections(tube(), centerlines([zCoordinates]))
    sections = result.CenterlineSections

    assert sections.GetNumberOfCells() == len(zCoordinates)
    area = sections.GetCellData().GetArray('CenterlineSectionArea')
    for i in range(sections.GetNumberOfCells()):
        assert area.GetValue(i) == pytest.approx(SECTION_AREA, rel=1e-6)


def test_point_where_plane_misses_surface():
    # A centerline point past the end of the tube gets no section, and zeros on the centerline
    zCoordinates = Z_BETWEEN_RINGS + [TUBE_LENGTH + 1.0]
    result = compute_sections(tube(), centerlines([zCoordinates]))
    sections = result.CenterlineSections

    assert sections.GetNumberOfCells() == len(Z_BETWEEN_RINGS)
    centerlinePointData = result.Centerlines.GetPointData()
    lastPointId = len(zCoordinates) - 1
    for name in ARRAY_NAMES:
        assert centerlinePointData.GetArray(name).GetTuple1(lastPointId) == 0
    assert centerlinePointData.GetArray('CenterlineSectionArea').GetTuple1(0) == pytest.approx(SECTION_AREA, rel=1e-6)


def test_point_shared_by_centerlines():
    # A point shared by two centerline cells gets a single section
    points = vtk.vtkPoints()
    for z in Z_BETWEEN_RINGS:
        points.InsertNextPoint(0.0, 0.0, z)
    lines = vtk.vtkCellArray()
    lines.InsertNextCell(5, [0, 1, 2, 3, 4])
    lines.InsertNextCell(6, [4, 5, 6, 7, 8, 9])
    polylines = vtk.vtkPolyData()
    polylines.SetPoints(points)
    polylines.SetLines(lines)

    sections = compute_sections(tube(), polylines).CenterlineSections

    assert sections.GetNumberOfCells() == len(Z_BETWEEN_RINGS)


@pytest.mark.parametrize('surfaceFixture,centerlineFixture,numberOfSections,numberOfClosedSections,totalArea', [
    ('aorta_surface', 'aorta_centerline', 409, 409, 54091.633),
    ('aorta_surface_branches', 'aorta_centerline_branches', 417, 410, 55321.155),
])
def test_aorta(surfaceFixture, centerlineFixture, numberOfSections, numberOfClosedSections, totalArea, request):
    # Both centerlines end with a duplicated point, which used to crash the filter
    surface = request.getfixturevalue(surfaceFixture)
    centerlinesCopy = vtk.vtkPolyData()
    centerlinesCopy.DeepCopy(request.getfixturevalue(centerlineFixture))

    sections = compute_sections(surface, centerlinesCopy).CenterlineSections

    assert sections.GetNumberOfCells() == numberOfSections
    cellData = sections.GetCellData()
    closed = cellData.GetArray('CenterlineSectionClosed')
    assert sum(closed.GetValue(i) for i in range(closed.GetNumberOfTuples())) == numberOfClosedSections
    area = cellData.GetArray('CenterlineSectionArea')
    assert area.GetValue(0) == pytest.approx(195.531177, rel=1e-6)
    assert sum(area.GetValue(i) for i in range(area.GetNumberOfTuples())) == pytest.approx(totalArea, rel=1e-6)
