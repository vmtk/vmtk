## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Analytic tests for vtkvmtkStaticTemporalStreamTracer: particles are traced
## through synthetic velocity fields whose trajectories are known in closed
## form, so no reference data files are needed.

import numpy as np
import pytest
import vtk
from vmtk import vtkvmtk


def make_velocity_grid(velocity_per_timestep):
    '''An 11x11x11 unit-spacing grid centered on the origin, with one uniform
    vector array "Velocity_<i>" per requested time step.'''
    grid = vtk.vtkImageData()
    grid.SetDimensions(11, 11, 11)
    grid.SetSpacing(1.0, 1.0, 1.0)
    grid.SetOrigin(-5.0, -5.0, -5.0)
    numberOfPoints = grid.GetNumberOfPoints()
    for index, velocity in enumerate(velocity_per_timestep):
        array = vtk.vtkDoubleArray()
        array.SetName('Velocity_%d' % index)
        array.SetNumberOfComponents(3)
        array.SetNumberOfTuples(numberOfPoints)
        for i in range(numberOfPoints):
            array.SetTuple3(i, *velocity)
        grid.GetPointData().AddArray(array)
    return grid


def make_time_steps_table(times):
    table = vtk.vtkTable()
    indexColumn = vtk.vtkIntArray()
    indexColumn.SetName('index')
    timeColumn = vtk.vtkDoubleArray()
    timeColumn.SetName('time')
    for i, t in enumerate(times):
        indexColumn.InsertNextValue(i)
        timeColumn.InsertNextValue(t)
    table.AddColumn(indexColumn)
    table.AddColumn(timeColumn)
    return table


def make_seed(point):
    seedPoints = vtk.vtkPoints()
    seedPoints.InsertNextPoint(point)
    seed = vtk.vtkPolyData()
    seed.SetPoints(seedPoints)
    return seed


def run_tracer(grid, table, seed, maximum_propagation):
    tracer = vtkvmtk.vtkvmtkStaticTemporalStreamTracer()
    tracer.SetInputData(grid)
    tracer.SetSourceData(seed)
    tracer.SetTimeStepsTable(table)
    tracer.SetVectorPrefix('Velocity_')
    tracer.SetSeedTime(0.0)
    tracer.SetIntegrationDirectionToForward()
    tracer.SetIntegratorTypeToRungeKutta4()
    tracer.SetMaximumPropagation(maximum_propagation)
    tracer.SetMaximumNumberOfSteps(10000)
    tracer.SetIntegrationStepUnit(1)  # LENGTH_UNIT: steps in arc length, not cell lengths
    tracer.SetInitialIntegrationStep(0.05)
    tracer.Update()
    return tracer.GetOutput()


def test_uniform_steady_flow_traces_straight_line():
    grid = make_velocity_grid([(1.0, 0.0, 0.0), (1.0, 0.0, 0.0)])
    table = make_time_steps_table([0.0, 1.0])
    streamline = run_tracer(grid, table, make_seed((-4.0, 0.0, 0.0)), 8.0)

    assert streamline.GetNumberOfCells() == 1
    points = np.array([streamline.GetPoint(i) for i in range(streamline.GetNumberOfPoints())])
    assert points.shape[0] > 10
    # straight line along +x: y and z never deviate
    assert np.abs(points[:, 1]).max() < 1e-10
    assert np.abs(points[:, 2]).max() < 1e-10
    # the full propagation length is covered
    assert points[-1, 0] == pytest.approx(4.0, abs=1e-6)
    # integration time equals distance for unit speed
    timeArray = streamline.GetPointData().GetArray('IntegrationTime')
    assert timeArray is not None
    assert timeArray.GetValue(streamline.GetNumberOfPoints() - 1) == pytest.approx(8.0, abs=1e-6)


def test_time_varying_flow_blends_velocity_between_timesteps():
    # The velocity field turns from +x at t=0 to +y at t=1, so at any time in
    # between the temporally interpolated field is ((1-t), t, 0) everywhere.
    # The propagation length keeps the trace inside the [0, 1] time interval
    # (the full turn has an arc length of ~0.81).
    grid = make_velocity_grid([(1.0, 0.0, 0.0), (0.0, 1.0, 0.0)])
    table = make_time_steps_table([0.0, 1.0])
    streamline = run_tracer(grid, table, make_seed((0.0, 0.0, 0.0)), 0.6)

    points = np.array([streamline.GetPoint(i) for i in range(streamline.GetNumberOfPoints())])
    timeArray = streamline.GetPointData().GetArray('IntegrationTime')
    times = np.array([timeArray.GetValue(i) for i in range(streamline.GetNumberOfPoints())])
    velocityArray = streamline.GetPointData().GetArray('Velocity')
    velocities = np.array([velocityArray.GetTuple3(i) for i in range(streamline.GetNumberOfPoints())])

    # The recorded velocity must be the exact temporal blend of the two
    # timestep fields at the recorded integration time (the field is
    # spatially uniform, so spatial interpolation cannot mask errors here).
    expected_velocities = np.column_stack([1.0 - times, times, np.zeros_like(times)])
    assert np.allclose(velocities, expected_velocities, atol=1e-9)

    # Trajectory sanity: the path curves monotonically from +x toward +y in
    # the z=0 plane, staying close to the closed-form trajectory
    # x(t) = t - t^2/2, y(t) = t^2/2. The comparison is loose because the
    # tracer reconstructs integration time from arc length, which is only
    # approximate when the speed varies along the path.
    assert np.abs(points[:, 2]).max() < 1e-10
    steps = np.diff(points, axis=0)
    assert (steps[:, 0] > 0.0).all()
    assert (steps[1:, 1] > 0.0).all()
    assert np.allclose(points[:, 0], times - times ** 2 / 2.0, atol=0.05)
    assert np.allclose(points[:, 1], times ** 2 / 2.0, atol=0.05)
