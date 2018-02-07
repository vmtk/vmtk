## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtksurfaceremeshing as remeshing


@pytest.mark.parametrize('elementsizemode,paramid', [
    ("area", '0'),
    ("edgelength", '1'),
])
def test_size_modes_with_default_params(aorta_surface, elementsizemode, compare_surfaces, paramid):
    name = __name__ + '_test_size_modes_with_default_params_' + paramid + '.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.ElementSizeMode = elementsizemode
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_target_area(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_target_area.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.TargetArea = 0.5
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_target_area_factor(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_target_area_factor.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.TargetAreaFactor = 1.0
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_triangle_split_factor(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_triangle_split_factor.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.TriangleSplitFactor = 7.0
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_max_area(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_max_area.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.MaxArea = 100
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_min_area(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_min_area.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.MinArea = 0.3
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_number_connectivity_iterations(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_number_connectivity_iterations.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.NumberOfConnectivityOptimizationIterations = 30
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_relaxation_factor(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_relaxation_factor.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.Relaxation = 0.7
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_min_area_factor(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_min_area_factor.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.MinAreaFactor = 0.7
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True


def test_change_aspect_ratio_threshold(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_aspect_ratio_threshold.vtp'
    remesher = remeshing.vmtkSurfaceRemeshing()
    remesher.Surface = aorta_surface
    remesher.AspectRatioThreshold = 1.7
    remesher.NumberOfIterations = 1
    remesher.Execute()

    assert compare_surfaces(remesher.Surface, name, tolerance=1.0) == True