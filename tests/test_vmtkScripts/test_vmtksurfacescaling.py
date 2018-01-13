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
import vmtk.vmtksurfacescaling as scaling

def test_isotropic_scale(aorta_surface, compare_surfaces):
    name = __name__ + '_test_isotropic_scale.vtp'
    scaler = scaling.vmtkSurfaceScaling()
    scaler.Surface = aorta_surface
    scaler.ScaleFactor = 2
    scaler.Execute()

    assert compare_surfaces(scaler.Surface, name, tolerance=1.0) == True


@pytest.mark.parametrize('xfactor,yfactor,zfactor,paramid', [
    (2, None, None, '0'),
    (None, 2, None, '1'),
    (None, None, 2, '2'),
    (2, 2, None, '3'),
    (2, None, 2, '4'),
    (None, 2, 2, '5'),
])
def test_xyz_scale_factors(aorta_surface, compare_surfaces, xfactor,
                           yfactor, zfactor, paramid):
    name = __name__ + '_test_xyz_scale_factors_' + paramid + '.vtp'
    scaler = scaling.vmtkSurfaceScaling()
    scaler.Surface = aorta_surface
    scaler.ScaleFactorX = xfactor
    scaler.ScaleFactorY = yfactor
    scaler.ScaleFactorZ = zfactor
    scaler.Execute()

    assert compare_surfaces(scaler.Surface, name, tolerance=1.0) == True


