## Program: VMTK
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

import pytest
from vmtk import vmtksurfaceimplicitdistance


def _array_values(surface, arrayname):
    arr = surface.GetPointData().GetArray(arrayname)
    return [arr.GetValue(i) for i in range(arr.GetNumberOfTuples())]


def test_distance_to_self_is_near_zero(aorta_surface):
    dist = vmtksurfaceimplicitdistance.vmtkSurfaceImplicitDistance()
    dist.Surface = aorta_surface
    dist.ReferenceSurface = aorta_surface
    dist.ArrayName = 'ImplicitDistance'
    dist.Execute()

    values = _array_values(dist.Surface, 'ImplicitDistance')
    assert len(values) == aorta_surface.GetNumberOfPoints()
    # every input point lies on the reference surface, so distance ~ 0
    assert max(abs(v) for v in values) < 1e-3


def test_unsigned_distance_is_nonnegative(aorta_surface, aorta_surface2):
    dist = vmtksurfaceimplicitdistance.vmtkSurfaceImplicitDistance()
    dist.Surface = aorta_surface
    dist.ReferenceSurface = aorta_surface2
    dist.ArrayName = 'ImplicitDistance'
    dist.ComputeSignedDistance = 0
    dist.Execute()

    values = _array_values(dist.Surface, 'ImplicitDistance')
    assert min(values) >= 0.0


def test_binary_output_uses_inside_outside_values(aorta_surface, aorta_surface2):
    dist = vmtksurfaceimplicitdistance.vmtkSurfaceImplicitDistance()
    dist.Surface = aorta_surface
    dist.ReferenceSurface = aorta_surface2
    dist.ArrayName = 'ImplicitDistance'
    dist.Binary = 1
    dist.InsideValue = 0.0
    dist.OutsideValue = 1.0
    dist.Execute()

    values = set(_array_values(dist.Surface, 'ImplicitDistance'))
    assert values.issubset({0.0, 1.0})


def test_distance_threshold_caps_values(aorta_surface, aorta_surface2):
    threshold = 2.0
    dist = vmtksurfaceimplicitdistance.vmtkSurfaceImplicitDistance()
    dist.Surface = aorta_surface
    dist.ReferenceSurface = aorta_surface2
    dist.ArrayName = 'ImplicitDistance'
    dist.DistanceThreshold = threshold
    dist.Execute()

    values = _array_values(dist.Surface, 'ImplicitDistance')
    assert max(abs(v) for v in values) <= threshold + 1e-9
