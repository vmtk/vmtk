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

import vmtk.vmtkboundarylabeler as boundarylabeler
from vmtk import vtkvmtk


LABELS = 'BoundaryLabels'
ORDER = 'BoundaryPointOrder'

TUBE_LENGTH = 6.0
TUBE_RADIUS = 1.0


def tube_surface(xOffset=0.0, radius=TUBE_RADIUS):
    '''An open-ended circular tube along z, with a boundary at z=0 and another at z=TUBE_LENGTH.'''
    numberOfCircumferentialPoints, numberOfAxialPoints = 24, 13
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfAxialPoints):
        z = TUBE_LENGTH * i / (numberOfAxialPoints - 1.0)
        for j in range(numberOfCircumferentialPoints):
            angle = 2.0 * math.pi * j / numberOfCircumferentialPoints
            points.InsertNextPoint(xOffset + radius * math.cos(angle), radius * math.sin(angle), z)
    for i in range(numberOfAxialPoints - 1):
        for j in range(numberOfCircumferentialPoints):
            p0 = i * numberOfCircumferentialPoints + j
            p1 = i * numberOfCircumferentialPoints + (j + 1) % numberOfCircumferentialPoints
            p2 = (i + 1) * numberOfCircumferentialPoints + j
            p3 = (i + 1) * numberOfCircumferentialPoints + (j + 1) % numberOfCircumferentialPoints
            polys.InsertNextCell(3, [p0, p1, p3])
            polys.InsertNextCell(3, [p0, p3, p2])
    surface = vtk.vtkPolyData()
    surface.SetPoints(points)
    surface.SetPolys(polys)
    return surface


@pytest.fixture(scope='function')
def tube():
    return tube_surface()


def label(surface, **kwargs):
    labeler = boundarylabeler.vmtkBoundaryLabeler()
    labeler.Surface = surface
    for name, value in kwargs.items():
        setattr(labeler, name, value)
    labeler.Execute()
    return labeler


def labels_of_boundaries(surface):
    '''The label of each boundary of surface, and the ordered point ids of each, read back from
    the arrays alone.'''
    boundaries = vtk.vtkPolyData()
    boundaryLabels = vtk.vtkIdList()
    assert vtkvmtk.vtkvmtkBoundaryLabels.GetBoundaries(surface, LABELS, ORDER, boundaries, boundaryLabels)
    pointIds = boundaries.GetPointData().GetScalars()
    rings = []
    for i in range(boundaries.GetNumberOfCells()):
        cell = boundaries.GetCell(i)
        numberOfPoints = cell.GetNumberOfPoints()
        rings.append([int(pointIds.GetTuple1(cell.GetPointId(j))) for j in range(numberOfPoints)])
    return [boundaryLabels.GetId(i) for i in range(boundaryLabels.GetNumberOfIds())], rings


def boundary_centroid_z(surface, ring):
    return sum(surface.GetPoint(pointId)[2] for pointId in ring) / float(len(ring))


def test_boundary_extraction_order_labels_every_boundary(tube):
    labeler = label(tube)

    assert labeler.NumberOfBoundaries == 2
    # a label is the cell entity id of the cap that will close its boundary, so the labels start
    # above the wall (entityidoffset, 1 by default) rather than at zero
    assert labeler.BoundaryLabels == [2, 3]
    assert labeler.UnmatchedPlaneLabels == []

    labels, rings = labels_of_boundaries(labeler.Surface)
    assert labels == [2, 3]
    # every point of each boundary is accounted for, and no interior point is claimed
    assert sorted(len(ring) for ring in rings) == [24, 24]


def test_boundary_extraction_order_follows_the_entity_id_offset(tube):
    """The labels are the ids the capper would have given the caps itself, so moving the wall
    moves them with it."""
    labeler = label(tube, CellEntityIdOffset=10)

    assert labeler.BoundaryLabels == [11, 12]


def test_extractor_lists_each_boundary_point_once(tube):
    """A boundary polyline is a ring whose points each appear once, with the first not repeated
    at the end. The extractor used to append that repeat whenever a test on two unrelated point
    ids happened to come out true, which put a duplicate into the (j+1) modulo n arithmetic every
    caller does: a barycenter pulled toward the duplicated point, and a degenerate triangle in
    the fan of a cap and the first layer of a flow extension."""
    extractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    extractor.SetInputData(tube)
    extractor.Update()
    boundaries = extractor.GetOutput()

    assert boundaries.GetNumberOfCells() == 2
    for i in range(boundaries.GetNumberOfCells()):
        cell = boundaries.GetCell(i)
        pointIds = [cell.GetPointId(j) for j in range(cell.GetNumberOfPoints())]
        assert len(pointIds) == len(set(pointIds))
        assert pointIds[0] != pointIds[-1]
        assert len(pointIds) == 24


def test_boundary_barycenter_is_not_pulled_off_axis(tube):
    """The bias the repeated point used to introduce, measured where it is easy to see: the
    barycenter of a boundary of this tube is on the axis, and a duplicated point would drag it
    off by about a twenty-fifth of the radius."""
    extractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    extractor.SetInputData(tube)
    extractor.Update()

    for i in range(extractor.GetOutput().GetNumberOfCells()):
        boundary = extractor.GetOutput().GetCell(i)
        barycenter = [0.0, 0.0, 0.0]
        vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(
            boundary.GetPoints(), barycenter)
        assert barycenter[0] == pytest.approx(0.0, abs=1e-9)
        assert barycenter[1] == pytest.approx(0.0, abs=1e-9)


def test_reconstruction_matches_the_extractor(tube):
    '''The load-bearing property: boundaries read back from the arrays are the ones the extractor
    would have produced, so a filter can use either and get the same answer.'''
    labeler = label(tube)

    extractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    extractor.SetInputData(labeler.Surface)
    extractor.Update()
    extracted = extractor.GetOutput()

    reconstructed = vtk.vtkPolyData()
    assert vtkvmtk.vtkvmtkBoundaryLabels.GetBoundaries(labeler.Surface, LABELS, ORDER, reconstructed, None)

    assert reconstructed.GetNumberOfCells() == extracted.GetNumberOfCells()
    assert reconstructed.GetNumberOfPoints() == extracted.GetNumberOfPoints()
    for i in range(extracted.GetNumberOfCells()):
        extractedCell = extracted.GetCell(i)
        reconstructedCell = reconstructed.GetCell(i)
        # same ring, same length, same order, same winding
        assert reconstructedCell.GetNumberOfPoints() == extractedCell.GetNumberOfPoints()
        for j in range(extractedCell.GetNumberOfPoints()):
            assert reconstructedCell.GetPointId(j) == extractedCell.GetPointId(j)
    for i in range(extracted.GetNumberOfPoints()):
        # the point scalars are the ids of the points in the surface, and have to agree too
        assert reconstructed.GetPointData().GetScalars().GetTuple1(i) == \
               extracted.GetPointData().GetScalars().GetTuple1(i)
        assert reconstructed.GetPoint(i) == pytest.approx(extracted.GetPoint(i))


@pytest.mark.parametrize('corruption', ['duplicate', 'gap', 'interpolated'])
def test_reconstruction_refuses_arrays_that_no_longer_describe_the_surface(tube, corruption):
    surface = label(tube).Surface
    order = surface.GetPointData().GetArray(ORDER)

    onBoundary = [i for i in range(surface.GetNumberOfPoints()) if order.GetTuple1(i) >= 0]
    if corruption == 'duplicate':
        # two points of one ring claiming the same place, as a merge of coincident points leaves it
        order.SetTuple1(onBoundary[1], order.GetTuple1(onBoundary[0]))
    elif corruption == 'gap':
        order.SetTuple1(onBoundary[1], 999)
    elif corruption == 'interpolated':
        # what a filter that interpolated the array onto a point it created leaves behind
        surface.GetPointData().GetArray(LABELS).SetTuple1(onBoundary[0], 0)
        order.SetTuple1(onBoundary[0], -1)

    boundaries = vtk.vtkPolyData()
    assert not vtkvmtk.vtkvmtkBoundaryLabels.GetBoundaries(surface, LABELS, ORDER, boundaries, None)
    assert boundaries.GetNumberOfCells() == 0


def test_closest_to_plane_origin_labels_the_nearest_boundary(tube):
    # one plane at each end of the tube, deliberately given labels that are not 0 and 1
    labeler = label(tube, LabelingMode='closesttoplaneorigin',
                    PlaneOrigins=[0.0, 0.0, TUBE_LENGTH, 0.0, 0.0, 0.0],
                    PlaneLabels=[7, 4])

    assert labeler.UnmatchedPlaneLabels == []
    labels, rings = labels_of_boundaries(labeler.Surface)
    byLabel = dict(zip(labels, rings))
    assert sorted(labels) == [4, 7]
    assert boundary_centroid_z(labeler.Surface, byLabel[7]) == pytest.approx(TUBE_LENGTH)
    assert boundary_centroid_z(labeler.Surface, byLabel[4]) == pytest.approx(0.0)


def test_closest_to_plane_origin_reports_a_plane_that_is_out_of_reach(tube):
    labeler = label(tube, LabelingMode='closesttoplaneorigin',
                    PlaneOrigins=[0.0, 0.0, 0.0, 0.0, 0.0, 1000.0],
                    PlaneLabels=[3, 9],
                    MaximumDistanceFromPlaneOrigin=2.0)

    assert labeler.UnmatchedPlaneLabels == [9]
    labels, _ = labels_of_boundaries(labeler.Surface)
    assert 3 in labels
    # the boundary the far plane did not reach is still labeled, with a fresh label of its own
    assert len(labels) == 2
    assert 9 not in labels


def test_closest_to_plane_origin_passes_over_a_boundary_not_entirely_within_reach(tube):
    '''The limit is on the whole boundary, not on the part of it that happens to be near: an
    origin can sit almost on top of one side of a ring and still not claim it, because the far
    side is out of reach.'''
    # 0.1 from the near side of the boundary at z=0, and 1.9 from the far side
    origin = [TUBE_RADIUS * 0.9, 0.0, 0.0]

    outOfReach = label(tube, LabelingMode='closesttoplaneorigin',
                       PlaneOrigins=origin, PlaneLabels=[3], MaximumDistanceFromPlaneOrigin=1.0)
    assert outOfReach.UnmatchedPlaneLabels == [3]

    # the same origin claims it once the limit takes in the whole ring
    withinReach = label(tube, LabelingMode='closesttoplaneorigin',
                        PlaneOrigins=origin, PlaneLabels=[3], MaximumDistanceFromPlaneOrigin=2.0)
    assert withinReach.UnmatchedPlaneLabels == []
    labels, rings = labels_of_boundaries(withinReach.Surface)
    byLabel = dict(zip(labels, rings))
    assert 3 in labels
    assert boundary_centroid_z(withinReach.Surface, byLabel[3]) == pytest.approx(0.0)


def test_on_plane_is_confined_to_the_neighbourhood_of_the_origin():
    '''A plane is infinite and a surface is not. Two tubes side by side have coplanar boundaries
    at z=0, and lying in the plane is not on its own enough to say which one a cut opened.'''
    near = tube_surface()
    far = tube_surface(xOffset=10.0)
    append = vtk.vtkAppendPolyData()
    append.AddInputData(near)
    append.AddInputData(far)
    append.Update()
    surface = append.GetOutput()

    labeler = label(surface, LabelingMode='onplane',
                    PlaneOrigins=[0.0, 0.0, 0.0],
                    PlaneNormals=[0.0, 0.0, 1.0],
                    PlaneLabels=[5],
                    MaximumDistanceFromPlane=1e-6,
                    MaximumDistanceFromPlaneOrigin=2.0)

    assert labeler.UnmatchedPlaneLabels == []
    labels, rings = labels_of_boundaries(labeler.Surface)
    byLabel = dict(zip(labels, rings))
    assert 5 in labels
    # the label went to the near tube's boundary, not to the coplanar one ten radii away
    meanX = sum(labeler.Surface.GetPoint(pointId)[0] for pointId in byLabel[5]) / float(len(byLabel[5]))
    assert meanX == pytest.approx(0.0, abs=0.5)


def test_on_plane_takes_the_boundary_nearest_the_origin_when_several_lie_in_the_plane():
    """With no locality limit, two tubes side by side both have a boundary in the plane z=0. The
    label has to go to the one nearest the plane's origin, not to whichever the extractor reached
    first, or which vessel end a clip point names would come down to the mesh's numbering."""
    for offset in (10.0, -10.0):
        append = vtk.vtkAppendPolyData()
        # built in both orders, so that the answer cannot be the extraction order in disguise
        append.AddInputData(tube_surface(xOffset=offset))
        append.AddInputData(tube_surface())
        append.Update()

        labeler = label(append.GetOutput(), LabelingMode='onplane',
                        PlaneOrigins=[0.0, 0.0, 0.0],
                        PlaneNormals=[0.0, 0.0, 1.0],
                        PlaneLabels=[5],
                        MaximumDistanceFromPlane=1e-6)

        assert labeler.UnmatchedPlaneLabels == []
        labels, rings = labels_of_boundaries(labeler.Surface)
        byLabel = dict(zip(labels, rings))
        meanX = sum(labeler.Surface.GetPoint(pointId)[0] for pointId in byLabel[5]) / float(len(byLabel[5]))
        assert meanX == pytest.approx(0.0, abs=0.5)
        # the other tube's coplanar boundary is not left nameless; it gets a label of its own
        assert len(labels) == 4


def test_on_plane_claims_only_a_boundary_lying_wholly_in_the_plane(tube):
    labeler = label(tube, LabelingMode='onplane',
                    PlaneOrigins=[0.0, 0.0, 0.0, 0.0, 0.0, TUBE_LENGTH * 0.5],
                    PlaneNormals=[0.0, 0.0, 1.0, 0.0, 0.0, 1.0],
                    PlaneLabels=[5, 6],
                    MaximumDistanceFromPlane=1e-6)

    # the second plane cuts the tube half way along, where there is no boundary at all
    assert labeler.UnmatchedPlaneLabels == [6]
    labels, rings = labels_of_boundaries(labeler.Surface)
    byLabel = dict(zip(labels, rings))
    assert 5 in labels
    assert boundary_centroid_z(labeler.Surface, byLabel[5]) == pytest.approx(0.0)


def test_matching_keeps_labels_through_cleaning(tube):
    labeled = label(tube, LabelingMode='closesttoplaneorigin',
                     PlaneOrigins=[0.0, 0.0, 0.0, 0.0, 0.0, TUBE_LENGTH],
                     PlaneLabels=[2, 8]).Surface

    clean = vtk.vtkCleanPolyData()
    clean.SetInputData(labeled)
    clean.Update()

    labels, rings = labels_of_boundaries(clean.GetOutput())
    byLabel = dict(zip(labels, rings))
    assert sorted(labels) == [2, 8]
    assert boundary_centroid_z(clean.GetOutput(), byLabel[2]) == pytest.approx(0.0)
    assert boundary_centroid_z(clean.GetOutput(), byLabel[8]) == pytest.approx(TUBE_LENGTH)


def test_matching_keeps_labels_through_smoothing(tube):
    labeled = label(tube, LabelingMode='closesttoplaneorigin',
                     PlaneOrigins=[0.0, 0.0, 0.0, 0.0, 0.0, TUBE_LENGTH],
                     PlaneLabels=[2, 8]).Surface

    smoother = vtk.vtkWindowedSincPolyDataFilter()
    smoother.SetInputData(labeled)
    smoother.SetNumberOfIterations(10)
    smoother.BoundarySmoothingOff()
    smoother.Update()

    labels, _ = labels_of_boundaries(smoother.GetOutput())
    assert sorted(labels) == [2, 8]


def test_matching_gives_a_newly_cut_boundary_a_label_of_its_own(tube):
    labeled = label(tube, LabelingMode='closesttoplaneorigin',
                     PlaneOrigins=[0.0, 0.0, 0.0, 0.0, 0.0, TUBE_LENGTH],
                     PlaneLabels=[2, 8]).Surface

    # cut the tube in half, taking away the boundary labeled 8 and opening a new one
    plane = vtk.vtkPlane()
    plane.SetOrigin(0.0, 0.0, TUBE_LENGTH * 0.5)
    plane.SetNormal(0.0, 0.0, -1.0)
    clipper = vtk.vtkClipPolyData()
    clipper.SetInputData(labeled)
    clipper.SetClipFunction(plane)
    clipper.Update()

    matched = label(clipper.GetOutput(), LabelingMode='matchexistinglabels').Surface

    labels, rings = labels_of_boundaries(matched)
    byLabel = dict(zip(labels, rings))
    assert len(labels) == 2
    # the boundary that came through untouched still answers to the label it was given
    assert 2 in labels
    assert boundary_centroid_z(matched, byLabel[2]) == pytest.approx(0.0)
    # the cut one is new, so it gets a label of its own rather than inheriting 8
    freshLabel = [boundaryLabel for boundaryLabel in labels if boundaryLabel != 2][0]
    assert freshLabel not in (2, 8)
    assert boundary_centroid_z(matched, byLabel[freshLabel]) == pytest.approx(TUBE_LENGTH * 0.5, abs=0.3)


OUTER_OFFSET = 1000


def walled_tube_surface(innerRadius=TUBE_RADIUS, outerRadius=TUBE_RADIUS * 1.4):
    '''Two concentric open tubes, the way a surface with a wall thickness comes out: four
    boundaries, an inner and an outer at each end.'''
    walled = vtk.vtkAppendPolyData()
    walled.AddInputData(tube_surface(radius=innerRadius))
    walled.AddInputData(tube_surface(radius=outerRadius))
    walled.Update()
    return walled.GetOutput()


def annular_labels(surface, offset=None):
    '''The boundary labels the filter gives surface with its annular mode on.'''
    labeler = vtkvmtk.vtkvmtkPolyDataBoundaryLabeler()
    labeler.SetInputData(surface)
    labeler.AnnularOn()
    if offset is not None:
        labeler.SetAnnularOuterBoundaryOffset(offset)
    labeler.Update()
    return [labeler.GetBoundaryLabels().GetId(i) for i in range(labeler.GetNumberOfBoundaries())]


def test_annular_is_off_by_default():
    """Nothing about a surface changes until the mode is asked for: the four boundaries of a
    walled tube are labelled one by one, as they always were."""
    labeler = vtkvmtk.vtkvmtkPolyDataBoundaryLabeler()
    labeler.SetInputData(walled_tube_surface())
    labeler.Update()

    assert labeler.GetAnnular() is False
    assert labeler.GetAnnularOuterBoundaryOffset() == OUTER_OFFSET
    assert sorted(labeler.GetBoundaryLabels().GetId(i) for i in range(4)) == [2, 3, 4, 5]


def test_annular_names_each_outer_boundary_after_its_inner_partner():
    """The pair of boundaries that bound one wall end read as one vessel end: the inner keeps the
    label it would have had, and the outer is that label plus the offset."""
    labels = annular_labels(walled_tube_surface())

    inner = sorted(label for label in labels if label < OUTER_OFFSET)
    outer = sorted(label for label in labels if label >= OUTER_OFFSET)
    assert len(inner) == 2 and len(outer) == 2
    # each outer label names an inner one, so the two of a wall differ by the offset alone
    assert [label - OUTER_OFFSET for label in outer] == inner


def test_annular_makes_the_inner_boundary_the_lower_id_of_a_pair():
    """What the arrangement is for: a filter that caps the annulus between a pair takes the lower
    of the two ids, and so always takes the inner boundary's, whichever way round it found them."""
    labels = annular_labels(walled_tube_surface())

    for label in labels:
        if label >= OUTER_OFFSET:
            assert label - OUTER_OFFSET in labels
            assert label - OUTER_OFFSET < label


def test_annular_offset_is_configurable():
    labels = annular_labels(walled_tube_surface(), offset=500)

    inner = sorted(label for label in labels if label < 500)
    outer = sorted(label for label in labels if label >= 500)
    assert len(inner) == 2 and len(outer) == 2
    assert [label - 500 for label in outer] == inner


def test_annular_leaves_a_boundary_with_no_partner_alone():
    """An odd boundary cannot be half of a wall. It keeps the label it was given rather than
    being paired with whatever was left."""
    surface = vtk.vtkAppendPolyData()
    surface.AddInputData(walled_tube_surface())
    lonePatch = vtk.vtkPlaneSource()
    lonePatch.SetOrigin(20.0, 20.0, 0.0)
    lonePatch.SetPoint1(21.0, 20.0, 0.0)
    lonePatch.SetPoint2(20.0, 21.0, 0.0)
    lonePatch.Update()
    surface.AddInputData(lonePatch.GetOutput())
    surface.Update()

    labels = annular_labels(surface.GetOutput())

    # five boundaries: two walls paired off, and one left with its own label
    assert len(labels) == 5
    assert len([label for label in labels if label >= OUTER_OFFSET]) == 2
    assert len([label for label in labels if label < OUTER_OFFSET]) == 3
