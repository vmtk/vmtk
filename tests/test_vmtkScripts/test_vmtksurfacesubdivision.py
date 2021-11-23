## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
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
import vmtk.vmtksurfacesubdivision as surfacesubdivision


@pytest.mark.parametrize("method,numsubdivisions,expected_points,expected_cells,paramid", [
    ('linear', 1, 25800, 51600, '0'),
    ('butterfly', 1, 25800, 51600, '1'),
    ('loop', 1, 25800, 51600, '2'),
    ('linear', 2, 103000, 206000, '3'),
    ('butterfly', 2, 103000, 206000, '4'),
    ('loop', 2, 103000, 206000, '5'),
])
def test_subdivision_methods_and_iterations(aorta_surface, compare_surfaces,
                                            method, numsubdivisions, paramid,
                                            expected_points, expected_cells):
    name = __name__ + '_test_subdivision_methods_and_iterations_' + paramid + '.vtp'
    subdivider = surfacesubdivision.vmtkSurfaceSubdivision()
    subdivider.Surface = aorta_surface
    subdivider.Method = method
    subdivider.NumberOfSubdivisions = numsubdivisions
    subdivider.Execute()

    assert compare_surfaces(subdivider.Surface, name) == True
    assert subdivider.Surface.GetNumberOfPoints() >= expected_points
    assert subdivider.Surface.GetNumberOfCells() >= expected_cells
