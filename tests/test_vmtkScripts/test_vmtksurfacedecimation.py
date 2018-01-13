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
import vmtk.vmtksurfacedecimation as surfacedecimation


def test_default_params(aorta_surface, compare_surfaces):
    name = __name__ + '_test_default_params.vtp'
    decimator = surfacedecimation.vmtkSurfaceDecimation()
    decimator.Surface = aorta_surface
    decimator.Execute()

    assert compare_surfaces(decimator.Surface, name) == True


def test_change_target_reduction(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_target_reduction.vtp'
    decimator = surfacedecimation.vmtkSurfaceDecimation()
    decimator.Surface = aorta_surface
    decimator.TargetReduction = 0.3
    decimator.Execute()

    assert compare_surfaces(decimator.Surface, name) == True