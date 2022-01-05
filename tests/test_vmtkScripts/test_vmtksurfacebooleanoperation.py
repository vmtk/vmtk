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

import pytest
import vmtk.vmtksurfacebooleanoperation as surfacebooleanoperation
import vtk


@pytest.mark.skipif(vtk.vtkVersion.GetVTKVersion() == '9.1.0', reason="requires vtk version != 9.1.0")
@pytest.mark.parametrize("operation,paramid", [
    ('intersection', '1'),
    ('difference', '2'),
])
def test_operations_default_tolerance_regression(aorta_surface2, aorta_surface_reference,
                                      operation, paramid, compare_surfaces):
    name = __name__ + '_test_operations_default_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface2
    booler.Surface2 = aorta_surface_reference
    booler.Operation = operation
    booler.Execute()

    assert compare_surfaces(booler.Surface, name, tolerance=1E-6) == True


@pytest.mark.skipif(vtk.vtkVersion.GetVTKVersion() != '9.1.0', reason="requires vtk == 9.1.0")
@pytest.mark.parametrize("operation,paramid", [
    ('intersection', '1'),
    ('difference', '2'),
])
def test_operations_default_tolerance_vtk_v9_1(aorta_surface2, aorta_surface_reference,
                                      operation, paramid, compare_surfaces):
    name = __name__ + '_test_operations_default_tolerance_' + paramid + '_vtk-9_1.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface2
    booler.Surface2 = aorta_surface_reference
    booler.Operation = operation
    booler.Execute()

    assert compare_surfaces(booler.Surface, name, tolerance=1E-6) == True


@pytest.mark.parametrize("operation,comp_tol,paramid", [
    ('union', 1e-6, '0'),
    ('intersection', 1e-1, '1'),
    ('difference', 1e-1, '2'),
])
def test_operations_default_tolerance(aorta_surface2, aorta_surface_reference,
                                      operation, comp_tol, paramid, compare_surfaces):
    name = __name__ + '_test_operations_default_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface2
    booler.Surface2 = aorta_surface_reference
    booler.Operation = operation
    booler.Execute()

    assert compare_surfaces(booler.Surface, name, tolerance=comp_tol) == True


@pytest.mark.parametrize("operation,tolerance,paramid", [
    ('union', 0.5, '0'),
    ('intersection', 0.5, '1'),
    ('difference', 0.5, '2'),
])
def test_operations_varied_tolerance(aorta_surface2, aorta_surface_reference,
                                      operation, tolerance, paramid, compare_surfaces):
    name = __name__ + '_test_operations_varied_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface2
    booler.Surface2 = aorta_surface_reference
    booler.Operation = operation
    booler.Tolerance = tolerance
    booler.Execute()

    assert compare_surfaces(booler.Surface, name, tolerance=1E-3) == True


@pytest.mark.parametrize("operation,paramid", [
    ('union', '0'),
    ('intersection', '1'),
    ('difference', '2'),
])
def test_operations_loop_method(aorta_surface2, aorta_surface_reference,
                                      operation, paramid, compare_surfaces):
    name = __name__ + '_test_operations_varied_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface2
    booler.Surface2 = aorta_surface_reference
    booler.Operation = operation
    booler.Method = 'loop'
    booler.Execute()
