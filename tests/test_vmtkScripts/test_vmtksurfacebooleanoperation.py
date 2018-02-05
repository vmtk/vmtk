## Program: VMTK
## Language:  Python
## Date:      January 12, 2018
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
import vmtk.vmtksurfacebooleanoperation as surfacebooleanoperation

@pytest.mark.skip(reason='this is currently failing')
@pytest.mark.parametrize("operation,paramid", [
    ('union', '0'),
    ('intersection', '1'),
    ('difference', '2'),
])
def test_operations_default_tolerance(aorta_surface, aorta_surface2,
                                      operation, paramid,
                                      write_surface, compare_surfaces):
    name = __name__ + '_test_operations_default_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface
    booler.Surface2 = aorta_surface2
    booler.Operation = operation
    booler.Execute()
    write_surface(booler.Surface, name)

    assert compare_surfaces(booler.Surface, name, tolerance=1E-6) == True

@pytest.mark.skip(reason='this is currently failing')
@pytest.mark.parametrize("operation,tolerance,paramid", [
    ('union', 0.5, '0'),
    ('intersection', 0.5, '1'),
    ('difference', 0.5, '2'),
])
def test_operations_varied_tolerance(aorta_surface, aorta_surface2,
                                      operation, tolerance, paramid,
                                      write_surface, compare_surfaces):
    name = __name__ + '_test_operations_varied_tolerance_' + paramid + '.vtp'
    booler = surfacebooleanoperation.vmtkSurfaceBooleanOperation()
    booler.Surface = aorta_surface
    booler.Surface2 = aorta_surface2
    booler.Operation = operation
    booler.Tolerance = tolerance
    booler.Execute()
    write_surface(booler.Surface, name)

    assert compare_surfaces(booler.Surface, name) == True