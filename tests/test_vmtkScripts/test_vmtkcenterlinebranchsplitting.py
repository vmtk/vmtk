## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import os

import numpy as np
import pytest
import vtk
from vtk.util.numpy_support import vtk_to_numpy

from vmtk import vtkvmtk


def read(input_datadir, fileName):
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName(os.path.join(input_datadir, fileName))
    reader.Update()
    polyData = vtk.vtkPolyData()
    polyData.DeepCopy(reader.GetOutput())
    return polyData


def point_array(polyData, name):
    array = polyData.GetPointData().GetArray(name)
    assert array is not None, name
    return vtk_to_numpy(array)


def branch_lengths(centerlines):
    branchIds = point_array(centerlines, 'BranchId')
    path = point_array(centerlines, 'Path')
    return [path[branchIds == branchId].max() for branchId in sorted(set(branchIds.tolist()) - {-1})]


def check_consistency(splitting):
    centerlines = splitting.GetCenterlines()
    numberOfPoints = centerlines.GetNumberOfPoints()

    # A tree of two-point lines with points numbered in order
    assert centerlines.GetNumberOfCells() == numberOfPoints - 1
    assert all(centerlines.GetCell(i).GetNumberOfPoints() == 2 for i in range(centerlines.GetNumberOfCells()))
    assert point_array(centerlines, 'GlobalNodeId').tolist() == list(range(numberOfPoints))

    # Each point is in exactly one branch or bifurcation
    branchIds = point_array(centerlines, 'BranchId')
    bifurcationIds = point_array(centerlines, 'BifurcationId')
    assert np.all((branchIds >= 0) != (bifurcationIds >= 0))

    # The path grows with the point ids in each branch and bifurcation. It is measured from the point
    # where the branch meets the bifurcation, which is part of the bifurcation, so it does not
    # always start from 0.
    path = point_array(centerlines, 'Path')
    for ids in [branchIds, bifurcationIds]:
        for regionId in set(ids.tolist()) - {-1}:
            regionPath = path[ids == regionId]
            assert regionPath[0] >= 0.0
            assert np.all(np.diff(regionPath) > 0.0)

    normals = point_array(centerlines, 'CenterlineSectionNormal')
    assert np.allclose(np.linalg.norm(normals, axis=1), 1.0)
    assert np.all(point_array(centerlines, 'CenterlineSectionArea') > 0.0)
    # Every point is on at least one of the input centerlines
    assert np.all(point_array(centerlines, 'CenterlineId').sum(axis=1) >= 1)

    # Arrays used only while the filter runs are removed
    for polyData in [centerlines, splitting.GetSurface()]:
        assert polyData.GetPointData().GetArray('BranchIdTmp') is None
        assert polyData.GetPointData().GetArray('BifurcationIdTmp') is None

    # Every surface point is labeled
    surface = splitting.GetSurface()
    surfaceBranchIds = point_array(surface, 'BranchId')
    surfaceBifurcationIds = point_array(surface, 'BifurcationId')
    assert np.all((surfaceBranchIds >= 0) | (surfaceBifurcationIds >= 0))
    assert set(surfaceBranchIds.tolist()) - {-1} == set(branchIds.tolist()) - {-1}

    sections = splitting.GetOutput()
    for name in ['CenterlineSectionArea', 'CenterlineSectionBifurcation', 'GlobalNodeId']:
        assert sections.GetCellData().GetArray(name).GetNumberOfTuples() == sections.GetNumberOfCells()


class Splitting:
    '''The results of a vtkvmtkPolyDataCenterlineBranchSplitting run and its progress events.'''

    def __init__(self, surface, centerlines, configure=None, abortAt=None):
        self.filter = vtkvmtk.vtkvmtkPolyDataCenterlineBranchSplitting()
        self.filter.SetInputData(surface)
        self.filter.SetCenterlines(centerlines)
        if configure:
            configure(self.filter)
        self.progress = []

        def onProgress(caller, event):
            self.progress.append(caller.GetProgress())
            if abortAt is not None and caller.GetProgress() >= abortAt:
                caller.SetAbortExecute(1)

        self.filter.AddObserver('ProgressEvent', onProgress)
        self.filter.Update()


# Splitting is slow, so each dataset is split once for all tests that need it
@pytest.fixture(scope='module')
def aorta_splitting(input_datadir):
    return Splitting(read(input_datadir, 'aorta-surface.vtp'), read(input_datadir, 'aorta-centerline.vtp'))


@pytest.fixture(scope='module')
def vmr_aorta_splitting(input_datadir):
    return Splitting(read(input_datadir, 'vmr-0159-aorta-surface.vtp'), read(input_datadir, 'vmr-0159-aorta-centerline.vtp'))


# Branch splitting results match SimVascular's implementation, except for sections where it counted
# labels that exist nowhere on the surface, produced by interpolating labels along the cut edges
@pytest.mark.parametrize('splittingFixture,numberOfPoints,branchPointCounts,bifurcationPointCounts,expectedBranchLengths', [
    ('aorta_splitting', 349,
        [61, 95, 85], [108], [28.505, 31.154, 29.022]),
    ('vmr_aorta_splitting', 1215,
        [152, 117, 57, 284, 194], [411], [8.332, 2.121, 1.630, 21.288, 5.982]),
])
def test_branch_splitting(splittingFixture, numberOfPoints, branchPointCounts, bifurcationPointCounts,
                          expectedBranchLengths, request):
    splitting = request.getfixturevalue(splittingFixture).filter
    centerlines = splitting.GetCenterlines()

    assert centerlines.GetNumberOfPoints() == numberOfPoints
    check_consistency(splitting)

    branchIds = point_array(centerlines, 'BranchId')
    bifurcationIds = point_array(centerlines, 'BifurcationId')
    assert [int((branchIds == i).sum()) for i in range(len(branchPointCounts))] == branchPointCounts
    assert [int((bifurcationIds == i).sum()) for i in range(len(bifurcationPointCounts))] == bifurcationPointCounts
    assert branch_lengths(centerlines) == pytest.approx(expectedBranchLengths, abs=1e-3)


def test_surface_normals_and_array_names(input_datadir, aorta_splitting):
    # aorta-surface.vtp has point normals that point into the vessel. Without them, and with other
    # array names, the results must be the same.
    surface = read(input_datadir, 'aorta-surface.vtp')
    surface.GetPointData().Initialize()
    centerlines = read(input_datadir, 'aorta-centerline.vtp')
    centerlines.GetPointData().GetArray('MaximumInscribedSphereRadius').SetName('Radius')

    def configure(splitting):
        splitting.SetRadiusArrayName('Radius')
        splitting.SetBranchIdArrayName('Branch')
        splitting.SetBifurcationIdArrayName('Bifurcation')
        splitting.SetPathArrayName('Distance')
        splitting.SetCenterlineSectionNormalArrayName('Normal')
        splitting.SetCenterlineSectionAreaArrayName('Area')

    renamed = Splitting(surface, centerlines, configure).filter
    default = aorta_splitting.filter

    for defaultName, name in [('BranchId', 'Branch'), ('BifurcationId', 'Bifurcation'), ('Path', 'Distance'),
                              ('CenterlineSectionNormal', 'Normal'), ('CenterlineSectionArea', 'Area'),
                              ('MaximumInscribedSphereRadius', 'Radius')]:
        assert np.array_equal(point_array(default.GetCenterlines(), defaultName), point_array(renamed.GetCenterlines(), name))
    for defaultName, name in [('BranchId', 'Branch'), ('BifurcationId', 'Bifurcation')]:
        assert np.array_equal(point_array(default.GetSurface(), defaultName), point_array(renamed.GetSurface(), name))
    assert renamed.GetOutput().GetCellData().GetArray('Area') is not None


def test_progress_and_abort(input_datadir, aorta_splitting):
    progress = aorta_splitting.progress
    assert len(progress) > 50
    assert progress[0] == 0.0 and progress[-1] == 1.0
    assert all(b >= a for a, b in zip(progress, progress[1:]))
    assert aorta_splitting.filter.GetProgressShift() == 0.0 and aorta_splitting.filter.GetProgressScale() == 1.0

    # Aborted while computing the sections: the output is empty
    aborted = Splitting(read(input_datadir, 'aorta-surface.vtp'), read(input_datadir, 'aorta-centerline.vtp'), abortAt=0.3)
    assert len(aborted.progress) < 50
    assert aborted.filter.GetOutput().GetNumberOfCells() == 0


def disconnected(centerlines):
    # Move the second centerline away from the first one
    result = vtk.vtkPolyData()
    result.DeepCopy(centerlines)
    ids = result.GetCell(1).GetPointIds()
    for k in range(ids.GetNumberOfIds()):
        x, y, z = result.GetPoint(ids.GetId(k))
        result.GetPoints().SetPoint(ids.GetId(k), x + 100.0, y, z)
    return result


def reversed_second_centerline(centerlines):
    result = vtk.vtkPolyData()
    result.DeepCopy(centerlines)
    lines = vtk.vtkCellArray()
    for i in range(centerlines.GetNumberOfCells()):
        ids = centerlines.GetCell(i).GetPointIds()
        pointIds = [ids.GetId(k) for k in range(ids.GetNumberOfIds())]
        if i == 1:
            pointIds.reverse()
        lines.InsertNextCell(len(pointIds), pointIds)
    result.SetLines(lines)
    return result


def without_radius(centerlines):
    result = vtk.vtkPolyData()
    result.DeepCopy(centerlines)
    result.GetPointData().RemoveArray('MaximumInscribedSphereRadius')
    return result


@pytest.mark.parametrize('makeInvalid', [disconnected, reversed_second_centerline, without_radius])
def test_invalid_centerlines(input_datadir, makeInvalid):
    errors = []
    observer = lambda caller, event: errors.append(event)
    splitting = vtkvmtk.vtkvmtkPolyDataCenterlineBranchSplitting()
    splitting.AddObserver('ErrorEvent', observer)
    splitting.GetExecutive().AddObserver('ErrorEvent', observer)
    splitting.SetInputData(read(input_datadir, 'aorta-surface.vtp'))
    splitting.SetCenterlines(makeInvalid(read(input_datadir, 'aorta-centerline.vtp')))
    splitting.Update()

    assert errors
    assert splitting.GetOutput().GetNumberOfCells() == 0
