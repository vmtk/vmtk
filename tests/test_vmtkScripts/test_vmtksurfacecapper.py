## Program: VMTK
## Language:  Python
## Date:      January 12, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import math
import os

import pytest
import vtk

import vmtk.vmtkboundarylabeler as boundarylabeler
import vmtk.vmtkflowextensions as flowextensions
import vmtk.vmtksurfacecapper as surfacecapper
from vmtk import vtkvmtk


@pytest.fixture(scope='module')
def aorta_surface_open_ends(input_datadir):
    import vmtk.vmtksurfacereader as surfacereader
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface-open-ends.stl')
    reader.Execute()
    return reader.Surface


@pytest.mark.skip(reason='cannot use this non-interactively')
@pytest.mark.parametrize('method,paramid', [
    ("simple", '0'),
    ("centerpoint", '1'),
    ("smooth", '2'),
    ("annular", '3'),
    ("concaveannular", '4'),
])
def test_methods_with_default_params(aorta_surface_open_ends, method, paramid, compare_surfaces, write_surface):
    name = __name__ + '_test_methods_with_default_params_' + paramid + '.vtp'
    capper = surfacecapper.vmtkSurfaceCapper()
    capper.Surface = aorta_surface_open_ends
    capper.Method = method
    capper.Interactive = 0
    capper.Execute()

    write_surface(capper.Surface, name)
    assert compare_surfaces(capper.Surface, name) == True


@pytest.mark.skip(reason='cannot use this non-interactively')
@pytest.mark.parametrize('constraint,rings,paramid', [
    (2.0, 8, '0'),
    (1.0, 12, '1'),
    (2.0, 12, '2'),
    (0.5, 8, '3')
])
def test_smooth_method_with_changing_params(aorta_surface_open_ends, constraint, rings,
                                            paramid, compare_surfaces, write_surface):
    name = __name__ + '_test_smooth_method_with_changing_params_' + paramid + '.vtp'
    capper = surfacecapper.vmtkSurfaceCapper()
    capper.Surface = aorta_surface_open_ends
    capper.Method = 'smooth'
    capper.ConstraintFactor = constraint
    capper.NumberOfRings = rings
    capper.Interactive = 0
    capper.Execute()

    write_surface(capper.Surface, name)
    assert compare_surfaces(capper.Surface, name) == True


LABELS = 'BoundaryLabels'
ORDER = 'BoundaryPointOrder'

TUBE_LENGTH = 6.0


def tube_surface():
    """An open-ended circular tube along z, with a boundary at z=0 and another at z=TUBE_LENGTH."""
    numberOfCircumferentialPoints, numberOfAxialPoints = 24, 13
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfAxialPoints):
        z = TUBE_LENGTH * i / (numberOfAxialPoints - 1.0)
        for j in range(numberOfCircumferentialPoints):
            angle = 2.0 * math.pi * j / numberOfCircumferentialPoints
            points.InsertNextPoint(math.cos(angle), math.sin(angle), z)
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


def labeled(surface, endPointLabels):
    """The surface with its two ends labeled, the first entry of endPointLabels going to the end at
    the low z bound and the second to the far end."""
    bounds = surface.GetBounds()
    labeler = boundarylabeler.vmtkBoundaryLabeler()
    labeler.Surface = surface
    labeler.LabelingMode = 'closesttoplaneorigin'
    labeler.PlaneOrigins = [0.0, 0.0, bounds[4], 0.0, 0.0, bounds[5]]
    labeler.PlaneLabels = list(endPointLabels)
    labeler.Execute()
    assert labeler.UnmatchedPlaneLabels == []
    return labeler.Surface


def cap(surface, **kwargs):
    capper = surfacecapper.vmtkSurfaceCapper()
    capper.Surface = surface
    capper.Method = 'centerpoint'
    capper.Interactive = 0
    for name, value in kwargs.items():
        setattr(capper, name, value)
    capper.Execute()
    return capper.Surface


def cap_z_by_entity_id(surface, wallCellCount):
    """The mean z of the cells carrying each cell entity id, over the cap cells only."""
    cellEntityIds = surface.GetCellData().GetArray('CellEntityIds')
    zByEntityId = {}
    for cellId in range(wallCellCount, surface.GetNumberOfCells()):
        cell = surface.GetCell(cellId)
        z = sum(cell.GetPoints().GetPoint(i)[2] for i in range(cell.GetNumberOfPoints()))
        z /= float(cell.GetNumberOfPoints())
        zByEntityId.setdefault(int(cellEntityIds.GetTuple1(cellId)), []).append(z)
    return dict((entityId, sum(zs) / float(len(zs))) for entityId, zs in zByEntityId.items())


def test_cap_takes_the_id_chosen_for_its_label(tube):
    wallCellCount = tube.GetNumberOfCells()
    surface = labeled(tube, [4, 7])

    # entry L is the id for the boundary labeled L; the ones in between are not asked for
    idsByLabel = [-1] * 8
    idsByLabel[4] = 104
    idsByLabel[7] = 107

    capped = cap(surface, BoundaryLabelsArrayName=LABELS, BoundaryPointOrderArrayName=ORDER,
                 BoundaryCellEntityIds=idsByLabel)

    capZ = cap_z_by_entity_id(capped, wallCellCount)
    assert sorted(capZ.keys()) == [104, 107]
    # label 4 was the end at z=0 and label 7 the far one, and the ids followed them
    assert capZ[104] == pytest.approx(0.0)
    assert capZ[107] == pytest.approx(TUBE_LENGTH)


def test_cap_keeps_its_positional_id_where_none_was_chosen(tube):
    wallCellCount = tube.GetNumberOfCells()
    surface = labeled(tube, [0, 1])

    capped = cap(surface, BoundaryLabelsArrayName=LABELS, BoundaryPointOrderArrayName=ORDER,
                 BoundaryCellEntityIds=[-1, 55])

    capZ = cap_z_by_entity_id(capped, wallCellCount)
    # the boundary labeled 1 took the id it was given, and the one labeled 0, whose entry is
    # negative, kept the id its position gives it: 0 + 1 + CellEntityIdOffset
    assert 55 in capZ
    assert capZ[55] == pytest.approx(TUBE_LENGTH)
    assert 0 + 1 + 1 in capZ


def test_a_chosen_id_is_free_of_the_offset(tube):
    wallCellCount = tube.GetNumberOfCells()
    surface = labeled(tube, [0, 1])

    capped = cap(surface, BoundaryLabelsArrayName=LABELS, BoundaryPointOrderArrayName=ORDER,
                 BoundaryCellEntityIds=[30, 31], CellEntityIdOffset=100)

    capZ = cap_z_by_entity_id(capped, wallCellCount)
    # the offset moves the ids this filter derives itself out of the way; it has no business
    # shifting one that was picked deliberately
    assert sorted(capZ.keys()) == [30, 31]


def test_an_id_chosen_for_a_vessel_end_survives_a_flow_extension(tube):
    """The scenario the whole thing exists for: name an end, extend it, and the cap of that same
    end still carries the id that was chosen for it -- even though the extension replaced the
    boundary with a new one, several radii away and made of points that did not exist before."""
    surface = labeled(tube, [4, 7])

    extensions = flowextensions.vmtkFlowExtensions()
    extensions.Surface = surface
    extensions.Interactive = 0
    extensions.ExtensionMode = 'boundarynormal'
    extensions.AdaptiveExtensionLength = 0
    extensions.ExtensionLength = 2.0
    extensions.BoundaryLabelsArrayName = LABELS
    extensions.BoundaryPointOrderArrayName = ORDER
    extensions.Execute()
    extended = extensions.Surface

    wallCellCount = extended.GetNumberOfCells()

    idsByLabel = [-1] * 8
    idsByLabel[4] = 104
    idsByLabel[7] = 107
    capped = cap(extended, BoundaryLabelsArrayName=LABELS, BoundaryPointOrderArrayName=ORDER,
                 BoundaryCellEntityIds=idsByLabel)

    capZ = cap_z_by_entity_id(capped, wallCellCount)
    assert sorted(capZ.keys()) == [104, 107]
    # the caps are out at the tips of the extensions, and each still carries its own end's id
    assert capZ[104] < -1.0
    assert capZ[107] > TUBE_LENGTH + 1.0


def test_unlabeled_input_is_capped_exactly_as_before(tube):
    """The label arrays are an addition, not a change: without them the filter works as it did."""
    plain = cap(tube)
    asked = cap(tube, BoundaryLabelsArrayName=LABELS, BoundaryPointOrderArrayName=ORDER)

    assert asked.GetNumberOfPoints() == plain.GetNumberOfPoints()
    assert asked.GetNumberOfCells() == plain.GetNumberOfCells()
    plainIds = plain.GetCellData().GetArray('CellEntityIds')
    askedIds = asked.GetCellData().GetArray('CellEntityIds')
    for cellId in range(plain.GetNumberOfCells()):
        assert askedIds.GetTuple1(cellId) == plainIds.GetTuple1(cellId)
