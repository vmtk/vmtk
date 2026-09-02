## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

"""The capping filters name their caps from the boundary labels of their input.

A cap's cell entity id is what tells a mesh generator, or anything else downstream, which vessel
end it is looking at. Without the labels that id can only be the boundary's position in the
extraction order, which changes whenever a filter in between renumbers or replaces a boundary.
With them the caller chooses an id per boundary label, and the choice survives.
"""

import math

import pytest
import vtk

from vmtk import vtkvmtk

try:
    from vmtk import vtkvmtkcontrib
except ImportError:
    try:
        # a build that compiled Contrib without laying out its python module, as the Slicer
        # extension build does; the wrapper module itself is still there
        import vtkvmtkContribPython as vtkvmtkcontrib
    except ImportError:  # built with VTK_VMTK_CONTRIB off
        vtkvmtkcontrib = None

LABELS = 'BoundaryLabels'
ORDER = 'BoundaryPointOrder'
CELL_ENTITY_IDS = 'CellEntityIds'
OUTER_OFFSET = 1000

TUBE_LENGTH = 6.0
TUBE_RADIUS = 1.0


def tube_surface(radius=TUBE_RADIUS):
    """An open-ended circular tube along z, with a boundary at each end."""
    numberOfCircumferentialPoints, numberOfAxialPoints = 24, 13
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfAxialPoints):
        z = TUBE_LENGTH * i / (numberOfAxialPoints - 1.0)
        for j in range(numberOfCircumferentialPoints):
            angle = 2.0 * math.pi * j / numberOfCircumferentialPoints
            points.InsertNextPoint(radius * math.cos(angle), radius * math.sin(angle), z)
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


def walled_tube_surface():
    """Two concentric tubes: four boundaries, an inner and an outer at each end."""
    walled = vtk.vtkAppendPolyData()
    walled.AddInputData(tube_surface(TUBE_RADIUS))
    walled.AddInputData(tube_surface(TUBE_RADIUS * 1.4))
    walled.Update()
    return walled.GetOutput()


def labelled(surface, annular=False):
    """surface with the boundary label arrays written on it, and the labels themselves."""
    labeler = vtkvmtk.vtkvmtkPolyDataBoundaryLabeler()
    labeler.SetInputData(surface)
    labeler.SetAnnular(annular)
    labeler.Update()
    labels = [labeler.GetBoundaryLabels().GetId(i) for i in range(labeler.GetNumberOfBoundaries())]
    return labeler.GetOutput(), labels


def chosen_ids(idByLabel, size=2048):
    """A BoundaryCellEntityIds array giving each label in idByLabel the id asked for, and no
    entry at all (-1) for every other."""
    ids = vtk.vtkIdTypeArray()
    ids.SetNumberOfTuples(size)
    for i in range(size):
        ids.SetValue(i, -1)
    for label, capId in idByLabel.items():
        ids.SetValue(label, capId)
    return ids


def cap_ids(capper, surface, useLabels=False, boundaryCellEntityIds=None):
    """The distinct cell entity ids of the capped surface."""
    capper.SetInputData(surface)
    capper.SetCellEntityIdsArrayName(CELL_ENTITY_IDS)
    capper.SetCellEntityIdOffset(0)
    if useLabels:
        capper.SetBoundaryLabelsArrayName(LABELS)
        capper.SetBoundaryPointOrderArrayName(ORDER)
    if boundaryCellEntityIds is not None:
        capper.SetBoundaryCellEntityIds(boundaryCellEntityIds)
    capper.Update()
    array = capper.GetOutput().GetCellData().GetArray(CELL_ENTITY_IDS)
    return sorted(set(int(array.GetTuple1(i)) for i in range(array.GetNumberOfTuples())))


SINGLE_BOUNDARY_CAPPERS = [
    pytest.param(vtkvmtk.vtkvmtkCapPolyData, id='centerpoint'),
    pytest.param(vtkvmtk.vtkvmtkSimpleCapPolyData, id='simple'),
    pytest.param(vtkvmtk.vtkvmtkSmoothCapPolyData, id='smooth'),
]

ANNULAR_CAPPERS = [
    pytest.param(vtkvmtk.vtkvmtkAnnularCapPolyData, id='annular'),
    pytest.param(
        vtkvmtkcontrib.vtkvmtkConcaveAnnularCapPolyData if vtkvmtkcontrib else None,
        id='concaveannular',
        marks=pytest.mark.skipif(vtkvmtkcontrib is None, reason='built with VTK_VMTK_CONTRIB off')),
]


@pytest.mark.parametrize('capperClass', SINGLE_BOUNDARY_CAPPERS)
def test_cap_takes_the_id_chosen_for_the_boundary_it_closes(capperClass):
    surface, labels = labelled(tube_surface())
    wanted = dict((label, 100 + label) for label in labels)

    ids = cap_ids(capperClass(), surface, useLabels=True,
                  boundaryCellEntityIds=chosen_ids(wanted))

    # 0 is the wall, which keeps the offset; each cap carries the id asked for by label
    assert ids == sorted(set([0]) | set(wanted.values()))


@pytest.mark.parametrize('capperClass', SINGLE_BOUNDARY_CAPPERS)
def test_cap_takes_the_boundary_label_when_no_id_is_chosen(capperClass):
    """The label is the boundary's name, so it is the cap's id when the caller names none: an
    id has to be asked for only where it should differ from the label."""
    surface, labels = labelled(tube_surface())

    ids = cap_ids(capperClass(), surface, useLabels=True)

    # 0 is the wall here as well as the label of the first boundary, so both caps are accounted
    # for by their labels
    assert ids == sorted(set([0]) | set(labels))


@pytest.mark.parametrize('capperClass', SINGLE_BOUNDARY_CAPPERS)
def test_cap_falls_back_to_its_label_where_no_id_was_chosen_for_it(capperClass):
    """An entry of -1 is no entry at all: that cap keeps its own label, so a caller can choose
    ids for the ends it cares about and leave the rest named after their boundaries."""
    surface, labels = labelled(tube_surface())

    ids = cap_ids(capperClass(), surface, useLabels=True,
                  boundaryCellEntityIds=chosen_ids({labels[0]: 55}))

    # the wall, the cap that was named, and one carrying its own label
    assert len(ids) == 3
    assert 55 in ids
    assert labels[1] in ids


@pytest.mark.parametrize('capperClass', SINGLE_BOUNDARY_CAPPERS)
def test_cap_ids_are_positional_when_the_labels_are_not_used(capperClass):
    """The long-standing behaviour, which everything already written depends on: with no label
    arrays named, the caps are numbered by the order the boundaries come out in."""
    ids = cap_ids(capperClass(), tube_surface())

    assert ids == [0, 1, 2]


def test_the_smooth_cap_is_made_of_triangles():
    """Whatever the ring geometry comes out as, the cells are triangles.

    The rings used to be closed with quadrilaterals, and the warp that places a ring can bring
    neighbouring points of it past each other, so that the quad through them crosses itself.
    vtkTriangleFilter gives up on such a cell and emits nothing for it, which takes a hole out of
    a cap that was closed -- and a caller triangulating the output, as the doc string tells it to,
    got back an open surface. Splitting the ring cells here means there is nothing left to fail
    on.
    """
    surface, _labels = labelled(tube_surface())

    capper = vtkvmtk.vtkvmtkSmoothCapPolyData()
    capper.SetInputData(surface)
    capper.SetConstraintFactor(0.0)
    capper.SetNumberOfRings(8)
    capper.Update()
    capped = capper.GetOutput()

    assert capped.GetNumberOfCells() > 0
    assert capped.GetPolys().IsHomogeneous() == 3

    # and triangulating it, which is what a caller does, leaves it as it was and still closed
    triangles = vtk.vtkTriangleFilter()
    triangles.SetInputData(capped)
    triangles.PassLinesOff()
    triangles.PassVertsOff()
    triangles.Update()
    assert triangles.GetOutput().GetNumberOfCells() == capped.GetNumberOfCells()

    for polyData in (capped, triangles.GetOutput()):
        featureEdges = vtk.vtkFeatureEdges()
        featureEdges.SetInputData(polyData)
        featureEdges.BoundaryEdgesOn()
        featureEdges.FeatureEdgesOff()
        featureEdges.NonManifoldEdgesOff()
        featureEdges.ManifoldEdgesOff()
        featureEdges.Update()
        assert featureEdges.GetOutput().GetNumberOfCells() == 0


@pytest.mark.parametrize('capperClass', ANNULAR_CAPPERS)
def test_annular_cap_takes_the_id_of_the_inner_boundary(capperClass):
    """A cap here closes a pair, so it has two labels to choose between. Labelled in annular mode
    the inner boundary is the lower of the two, and the cap is named after it -- the vessel end
    the caller means, rather than whichever boundary the pairing reached first."""
    surface, labels = labelled(walled_tube_surface(), annular=True)
    wanted = dict((label, 700 + label) for label in labels if label < OUTER_OFFSET)

    ids = cap_ids(capperClass(), surface, useLabels=True,
                  boundaryCellEntityIds=chosen_ids(wanted))

    assert ids == sorted(set([0]) | set(wanted.values()))


@pytest.mark.parametrize('capperClass', ANNULAR_CAPPERS)
def test_annular_cap_prefers_the_lower_boundary_id_when_both_are_named(capperClass):
    """Both boundaries of a pair named, and named differently. The cap takes the lower one's id
    rather than whichever the pairing happened to see first, which the caller cannot predict."""
    surface, labels = labelled(walled_tube_surface(), annular=True)
    wanted = {}
    for label in labels:
        wanted[label] = 700 + label if label < OUTER_OFFSET else 900 + (label - OUTER_OFFSET)

    ids = cap_ids(capperClass(), surface, useLabels=True,
                  boundaryCellEntityIds=chosen_ids(wanted))

    innerIds = [700 + label for label in labels if label < OUTER_OFFSET]
    assert ids == sorted(set([0]) | set(innerIds))


@pytest.mark.parametrize('capperClass', ANNULAR_CAPPERS)
def test_annular_cap_ids_are_positional_when_the_labels_are_not_used(capperClass):
    ids = cap_ids(capperClass(), walled_tube_surface())

    assert ids == [0, 1, 2]
