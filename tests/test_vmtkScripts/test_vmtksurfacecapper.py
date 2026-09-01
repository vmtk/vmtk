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
    write_surface(capper.Surface)

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
    write_surface(capper.Surface)

    assert compare_surfaces(capper.Surface, name) == True


def open_tube_surface():
    '''An open-ended circular tube extruded along z, so that it has exactly two boundaries.'''
    length = 6.0
    numberOfCircumferentialPoints, numberOfAxialPoints = 32, 8
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfAxialPoints):
        z = length * i / (numberOfAxialPoints - 1.0)
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


@pytest.fixture(scope='module')
def open_tube():
    return open_tube_surface()


def cap(surface, **kwargs):
    capper = surfacecapper.vmtkSurfaceCapper()
    capper.Surface = surface
    capper.Method = 'centerpoint'
    capper.Interactive = 0
    capper.TriangleOutput = 0
    for name, value in kwargs.items():
        setattr(capper, name, value)
    capper.Execute()
    return capper.Surface


def cap_entity_ids(surface):
    '''The set of entity ids the caps carry, and the id left on the cells that were already there.'''
    ids = surface.GetCellData().GetArray('CellEntityIds')
    values = [int(ids.GetTuple1(i)) for i in range(ids.GetNumberOfTuples())]
    wallId = values[0]
    return sorted(set(values) - {wallId}), wallId


def test_caps_are_numbered_by_boundary_position_by_default(open_tube):
    capIds, wallId = cap_entity_ids(cap(open_tube))

    assert wallId == 1                  # the CellEntityIdOffset the script defaults to
    assert capIds == [2, 3]             # boundary index + 1 + offset


def test_cell_entity_ids_choose_the_id_of_each_cap(open_tube):
    capIds, wallId = cap_entity_ids(cap(open_tube, CellEntityIds=[7, 9]))

    assert wallId == 1
    assert capIds == [7, 9]


def test_cell_entity_id_offset_is_not_applied_to_a_chosen_id(open_tube):
    """The offset is the id of the cells copied from the input and the base the derived cap ids
    count up from, but a cap named in CellEntityIds takes that id as it stands."""
    capped = cap(open_tube, CellEntityIdOffset=100, CellEntityIds=[7, -1])
    capIds, wallId = cap_entity_ids(capped)

    assert wallId == 100                # the cells that were already there
    assert capIds == [7, 102]           # the chosen id verbatim, and 1 + 1 + 100 for the other


def test_cell_entity_ids_fall_back_where_the_caller_gave_none(open_tube):
    # -1 asks for the positional id, and so does a boundary past the end of the list
    byPosition, _ = cap_entity_ids(cap(open_tube, CellEntityIds=[-1, -1]))
    firstOnly, _ = cap_entity_ids(cap(open_tube, CellEntityIds=[7]))

    assert byPosition == [2, 3]
    assert firstOnly == [3, 7]


def test_cell_entity_ids_survive_a_flow_extension(open_tube):
    '''The point of the two features together: an id chosen for a vessel end still lands on that
    end after flow extensions have replaced its boundary with one several radii down the branch.'''
    chosenIds = [7, 9]

    extensions = flowextensions.vmtkFlowExtensions()
    extensions.Surface = open_tube
    extensions.Interactive = 0
    extensions.ExtensionMode = 'boundarynormal'
    extensions.AdaptiveExtensionLength = 0
    extensions.ExtensionLength = 3.0
    extensions.Execute()

    # the extension replaced each boundary, so the ids have to be carried onto the ones that
    # replaced them before the capper, which knows nothing of the extension, is given them
    extendedIds = [-1] * len(extensions.OutputBoundaryIds)
    for boundaryId, outputBoundaryId in enumerate(extensions.OutputBoundaryIds):
        extendedIds[outputBoundaryId] = chosenIds[boundaryId]

    capIds, wallId = cap_entity_ids(cap(extensions.Surface, CellEntityIds=extendedIds))

    assert capIds == sorted(chosenIds)

    # and each cap really is at the end of the extension grown from the boundary it was chosen for
    barycenters = []
    boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    boundaryExtractor.SetInputData(open_tube)
    boundaryExtractor.Update()
    for i in range(boundaryExtractor.GetOutput().GetNumberOfCells()):
        barycenter = [0.0, 0.0, 0.0]
        vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(
            boundaryExtractor.GetOutput().GetCell(i).GetPoints(), barycenter)
        barycenters.append(barycenter)

    capped = cap(extensions.Surface, CellEntityIds=extendedIds)
    ids = capped.GetCellData().GetArray('CellEntityIds')
    for boundaryId, chosenId in enumerate(chosenIds):
        z = []
        for cellId in range(capped.GetNumberOfCells()):
            if int(ids.GetTuple1(cellId)) == chosenId:
                bounds = capped.GetCell(cellId).GetBounds()
                z.append(0.5 * (bounds[4] + bounds[5]))
        capZ = sum(z) / len(z)
        # 3 mm past the boundary it was chosen for, on that boundary's side of the tube
        assert abs(capZ - barycenters[boundaryId][2]) == pytest.approx(3.0, rel=0.15)
