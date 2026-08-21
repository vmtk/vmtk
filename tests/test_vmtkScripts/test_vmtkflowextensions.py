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

import vmtk.vmtkflowextensions as flowextensions
from vmtk import vtkvmtk


@pytest.fixture(scope='module')
def elliptic_tube():
    '''An open-ended tube with a 2:1 elliptic cross-section, extruded along the z axis.

    Its profiles depart from a circle much more than any of the test surfaces, which makes the
    difference between a preserved and a circularized cross-section easy to measure.
    '''
    semiAxisX, semiAxisY, length = 2.0, 1.0, 6.0
    numberOfCircumferentialPoints, numberOfAxialPoints = 48, 12
    points = vtk.vtkPoints()
    polys = vtk.vtkCellArray()
    for i in range(numberOfAxialPoints):
        z = length * i / (numberOfAxialPoints - 1.0)
        for j in range(numberOfCircumferentialPoints):
            angle = 2.0 * math.pi * j / numberOfCircumferentialPoints
            points.InsertNextPoint(semiAxisX * math.cos(angle), semiAxisY * math.sin(angle), z)
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


def boundary_profiles(surface):
    '''(radius ratio, mean radius) of each open boundary, measured about its barycenter.

    The radius ratio is 1 for a circular profile and 2 for a 2:1 elliptic one, so it describes the
    shape of a profile independently of its size and position. Profiles come in the order used by
    vtkvmtkPolyDataBoundaryExtractor, which is the order the flow extensions filter works in.
    '''
    boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    boundaryExtractor.SetInputData(surface)
    boundaryExtractor.Update()
    boundaries = boundaryExtractor.GetOutput()

    profiles = []
    for i in range(boundaries.GetNumberOfCells()):
        points = boundaries.GetCell(i).GetPoints()
        barycenter = [0.0, 0.0, 0.0]
        vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(points, barycenter)
        radii = []
        for j in range(points.GetNumberOfPoints()):
            point = points.GetPoint(j)
            radii.append(math.sqrt(sum((point[k] - barycenter[k]) ** 2 for k in range(3))))
        profiles.append((max(radii) / min(radii), sum(radii) / len(radii)))
    return profiles


def extend(surface, preserveshape, centerlines=None, **kwargs):
    extensions = flowextensions.vmtkFlowExtensions()
    extensions.Surface = surface
    extensions.PreserveCrossSectionShape = preserveshape
    extensions.Interactive = 0
    if centerlines is not None:
        extensions.Centerlines = centerlines
        extensions.ExtensionMode = 'centerlinedirection'
    else:
        extensions.ExtensionMode = 'boundarynormal'
    for name, value in kwargs.items():
        setattr(extensions, name, value)
    extensions.Execute()
    return extensions.Surface


def test_extensions_are_appended_to_the_surface(elliptic_tube):
    surface = extend(elliptic_tube, 0)

    assert surface.GetNumberOfPoints() > elliptic_tube.GetNumberOfPoints()
    assert surface.GetNumberOfCells() > elliptic_tube.GetNumberOfCells()
    # the extensions are left open, one new profile per original open profile
    assert len(boundary_profiles(surface)) == len(boundary_profiles(elliptic_tube))


def test_cross_section_is_circularized_by_default(elliptic_tube):
    assert all(ratio == pytest.approx(2.0, rel=0.01) for ratio, _ in boundary_profiles(elliptic_tube))

    for ratio, _ in boundary_profiles(extend(elliptic_tube, 0)):
        assert ratio == pytest.approx(1.0, rel=0.01)


def test_cross_section_shape_is_preserved_when_requested(elliptic_tube):
    for ratio, _ in boundary_profiles(extend(elliptic_tube, 1)):
        assert ratio == pytest.approx(2.0, rel=0.02)


def test_preserved_cross_section_is_scaled_to_the_extension_radius(elliptic_tube):
    surface = extend(elliptic_tube, 1, AdaptiveExtensionRadius=0, ExtensionRadius=1.0)

    for ratio, meanRadius in boundary_profiles(surface):
        assert ratio == pytest.approx(2.0, rel=0.02)
        assert meanRadius == pytest.approx(1.0, rel=0.02)


def test_cross_section_shape_is_preserved_along_a_centerline(aorta_surface_openends, aorta_centerline):
    # the extension direction is the centerline tangent, which is generally not orthogonal to the
    # boundary, so the extension has to be built from the projected outline
    inputProfiles = boundary_profiles(aorta_surface_openends)
    circularized = boundary_profiles(extend(aorta_surface_openends, 0, centerlines=aorta_centerline))
    preserved = boundary_profiles(extend(aorta_surface_openends, 1, centerlines=aorta_centerline))

    assert len(circularized) == len(inputProfiles)
    assert len(preserved) == len(inputProfiles)
    for (inputRatio, _), (circularizedRatio, _), (preservedRatio, _) in zip(inputProfiles, circularized, preserved):
        assert inputRatio > 1.1
        assert circularizedRatio == pytest.approx(1.0, rel=0.01)
        assert preservedRatio == pytest.approx(inputRatio, rel=0.2)
