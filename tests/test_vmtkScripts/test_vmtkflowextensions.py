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


def elliptic_tube_surface(semiAxisX, semiAxisY):
    '''An open-ended tube with an elliptic cross-section of the given semi-axes, extruded along z.'''
    length = 6.0
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


@pytest.fixture(scope='module')
def elliptic_tube():
    '''An open-ended tube with a 2:1 elliptic cross-section, extruded along the z axis.

    Its profiles depart from a circle much more than any of the test surfaces, which makes the
    difference between a preserved and a circularized cross-section easy to measure.
    '''
    return elliptic_tube_surface(2.0, 1.0)


@pytest.fixture(scope='module')
def flat_tube():
    '''An open-ended tube with a 6:1 elliptic cross-section, extruded along the z axis.

    Flat enough that most of what makes its profile that shape sits in the high circumferential
    frequencies, which is where the transition interpolation modes differ.
    '''
    return elliptic_tube_surface(6.0, 1.0)


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


def run_extensions(surface, preserveshape, centerlines=None, **kwargs):
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
    return extensions


def extend(surface, preserveshape, centerlines=None, **kwargs):
    return run_extensions(surface, preserveshape, centerlines, **kwargs).Surface


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


def extension_ring_ratios(inputSurface, surface, numberOfRingPoints, numberOfRings):
    '''Radius ratio of the first rings of the extension grown on the first open boundary.

    The filter copies the input points first and then appends one ring of numberOfRingPoints points
    per layer, boundary after boundary, so the rings of the first extension follow the input points
    directly. Reading them one by one shows how the cross-section changes along the transition,
    which the profile of the finished extension alone does not.
    '''
    firstExtensionPointId = inputSurface.GetNumberOfPoints()
    ratios = []
    for ring in range(numberOfRings):
        points = [surface.GetPoint(firstExtensionPointId + ring * numberOfRingPoints + i)
                  for i in range(numberOfRingPoints)]
        barycenter = [sum(point[k] for point in points) / len(points) for k in range(3)]
        radii = [math.sqrt(sum((point[k] - barycenter[k]) ** 2 for k in range(3))) for point in points]
        ratios.append(max(radii) / min(radii))
    return ratios


# a transition resolved into enough layers for its profile to be read off ring by ring
TRANSITION = dict(AdaptiveExtensionLength=0, ExtensionLength=6.0, TransitionRatio=0.5)


def test_ramp_transition_starts_at_the_real_cross_section(flat_tube):
    numberOfRingPoints = 200
    options = dict(TargetNumberOfBoundaryPoints=numberOfRingPoints, **TRANSITION)

    splined = extend(flat_tube, 0, InterpolationMode='thinplatespline', **options)
    ramped = extend(flat_tube, 0, InterpolationMode='ramp', **options)

    # The boundary is 6:1. The ramp fades the displacement onto it out from full, so the extension
    # starts on it, while the thin plate spline reproduces it only where it is pinned and loses the
    # high circumferential frequencies that make a section flat as soon as it moves away.
    assert extension_ring_ratios(flat_tube, ramped, numberOfRingPoints, 1)[0] > 5.5
    assert extension_ring_ratios(flat_tube, splined, numberOfRingPoints, 1)[0] < 4.5


def test_ramp_transition_spans_the_requested_length(elliptic_tube):
    numberOfRingPoints = 50
    options = dict(TargetNumberOfBoundaryPoints=numberOfRingPoints, InterpolationMode='ramp',
                   AdaptiveExtensionLength=0, ExtensionLength=6.0)

    longTransition = extension_ring_ratios(
        elliptic_tube, extend(elliptic_tube, 0, TransitionRatio=0.5, **options), numberOfRingPoints, 16)
    shortTransition = extension_ring_ratios(
        elliptic_tube, extend(elliptic_tube, 0, TransitionRatio=0.25, **options), numberOfRingPoints, 16)

    # the cross-section goes from the boundary to the circle once, without turning back
    assert all(longTransition[i] >= longTransition[i+1] for i in range(len(longTransition)-1))
    # halving the transition halves the number of rings it takes: the short one is already a circle
    # where the long one is only half way through
    assert shortTransition[7] == pytest.approx(1.0, rel=0.01)
    assert longTransition[7] > 1.3
    assert longTransition[15] == pytest.approx(1.0, rel=0.01)


def test_linear_interpolation_mode_blends(elliptic_tube):
    numberOfRingPoints = 50
    surface = extend(elliptic_tube, 0, InterpolationMode='linear',
                     TargetNumberOfBoundaryPoints=numberOfRingPoints, **TRANSITION)

    # the linear mode used to be an empty branch, which left the first ring the plain circle the
    # extension is swept from, that is a ratio of 1
    ratios = extension_ring_ratios(elliptic_tube, surface, numberOfRingPoints, 16)
    assert ratios[0] > 1.8
    assert ratios[15] == pytest.approx(1.0, rel=0.01)


def extension_growths(inputSurface, surface):
    '''How far the surface grew past the input at either end of the z axis, in ascending order.

    The tube fixtures are extruded along z and extended along their boundary normals, so each
    extension's length is the growth of the z extent on its side. Sorting makes the result
    independent of the order the boundary extractor happens to number the two boundaries in.
    '''
    inputBounds = inputSurface.GetBounds()
    bounds = surface.GetBounds()
    return sorted([inputBounds[4] - bounds[4], bounds[5] - inputBounds[5]])


def test_extension_length_scale_factors_scale_each_extension(elliptic_tube):
    options = dict(AdaptiveExtensionLength=0, ExtensionLength=2.0)

    unscaled = extension_growths(elliptic_tube, extend(elliptic_tube, 0, **options))
    scaled = extension_growths(
        elliptic_tube, extend(elliptic_tube, 0, ExtensionLengthScaleFactors=[2.0, 0.5], **options))
    partial = extension_growths(
        elliptic_tube, extend(elliptic_tube, 0, ExtensionLengthScaleFactors=[3.0], **options))

    # extensions are built in whole layers, so lengths match the request only to within a layer
    assert all(growth == pytest.approx(2.0, rel=0.1) for growth in unscaled)
    assert scaled[0] == pytest.approx(0.5 * 2.0, rel=0.15)
    assert scaled[1] == pytest.approx(2.0 * 2.0, rel=0.1)
    # a boundary beyond the end of the list keeps its unscaled length
    assert partial[0] == pytest.approx(2.0, rel=0.1)
    assert partial[1] == pytest.approx(3.0 * 2.0, rel=0.1)


def test_ramp_leaves_a_preserved_cross_section_alone(elliptic_tube):
    numberOfRingPoints = 50
    surface = extend(elliptic_tube, 1, InterpolationMode='ramp',
                     TargetNumberOfBoundaryPoints=numberOfRingPoints, **TRANSITION)

    # nothing to morph: every ring, the transition included, is the 2:1 boundary outline
    for ratio in extension_ring_ratios(elliptic_tube, surface, numberOfRingPoints, 8):
        assert ratio == pytest.approx(2.0, rel=0.02)


def boundary_barycenters(surface):
    '''The barycenter of each open boundary, in boundary extraction order.'''
    boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
    boundaryExtractor.SetInputData(surface)
    boundaryExtractor.Update()
    boundaries = boundaryExtractor.GetOutput()
    barycenters = []
    for i in range(boundaries.GetNumberOfCells()):
        barycenter = [0.0, 0.0, 0.0]
        vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(
            boundaries.GetCell(i).GetPoints(), barycenter)
        barycenters.append(barycenter)
    return barycenters


def test_output_boundary_ids_account_for_every_boundary(elliptic_tube):
    extensions = run_extensions(elliptic_tube, 0, AdaptiveExtensionLength=0, ExtensionLength=2.0)

    # the tube has two open boundaries, and each is replaced by the one at the tip of its extension
    assert len(extensions.OutputBoundaryIds) == len(boundary_barycenters(elliptic_tube)) == 2
    assert sorted(extensions.OutputBoundaryIds) == [0, 1]


def test_output_boundary_ids_point_at_the_boundary_that_replaced_each_one(elliptic_tube):
    extensions = run_extensions(elliptic_tube, 0, AdaptiveExtensionLength=0, ExtensionLength=2.0)

    before = boundary_barycenters(elliptic_tube)
    after = boundary_barycenters(extensions.Surface)
    # the tube is extruded along z and extended along its boundary normals, so a boundary and its
    # replacement sit on the same axis, the replacement 2 mm further out
    for boundaryId, outputBoundaryId in enumerate(extensions.OutputBoundaryIds):
        assert after[outputBoundaryId][0] == pytest.approx(before[boundaryId][0], abs=1e-6)
        assert after[outputBoundaryId][1] == pytest.approx(before[boundaryId][1], abs=1e-6)
        assert abs(after[outputBoundaryId][2] - before[boundaryId][2]) == pytest.approx(2.0, rel=0.1)


def test_output_boundary_ids_are_not_simply_the_input_ids(elliptic_tube):
    # Extending one boundary of two leaves the other where it is, so it is met first when the output
    # is walked for boundaries and the ids no longer line up with the input: the case a caller that
    # assumed the extraction order was preserved would get wrong.
    before = boundary_barycenters(elliptic_tube)
    mappings = []
    for extendOnly in range(len(before)):
        boundaryIds = vtk.vtkIdList()
        boundaryIds.InsertNextId(extendOnly)
        extensionsFilter = vtkvmtk.vtkvmtkPolyDataFlowExtensionsFilter()
        extensionsFilter.SetInputData(elliptic_tube)
        extensionsFilter.SetExtensionModeToUseNormalToBoundary()
        extensionsFilter.SetAdaptiveExtensionLength(0)
        extensionsFilter.SetExtensionLength(2.0)
        extensionsFilter.SetAdaptiveExtensionRadius(1)
        extensionsFilter.SetBoundaryIds(boundaryIds)
        extensionsFilter.Update()

        reported = extensionsFilter.GetOutputBoundaryIds()
        mapping = [reported.GetId(i) for i in range(reported.GetNumberOfIds())]
        mappings.append(mapping)

        assert sorted(mapping) == list(range(len(before)))
        after = boundary_barycenters(extensionsFilter.GetOutput())
        for boundaryId, outputBoundaryId in enumerate(mapping):
            grown = abs(after[outputBoundaryId][2] - before[boundaryId][2])
            assert grown == pytest.approx(2.0 if boundaryId == extendOnly else 0.0, abs=0.3)

    assert any(mapping != list(range(len(before))) for mapping in mappings)
