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
import vmtk.vmtksurfacecapper as surfacecapper


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
