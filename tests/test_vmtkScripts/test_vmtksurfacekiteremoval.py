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
import vmtk.vmtksurfacekiteremoval as kiteremoval


def test_default_params(aorta_surface, compare_surfaces):
    name = __name__ + '_test_default_params.vtp'
    remover = kiteremoval.vmtkSurfaceKiteRemoval()
    remover.Surface = aorta_surface
    remover.Execute()

    assert compare_surfaces(remover.Surface, name) == True


def test_change_size_factor(aorta_surface, compare_surfaces):
    name = __name__ + '_test_change_size_factor.vtp'
    remover = kiteremoval.vmtkSurfaceKiteRemoval()
    remover.Surface = aorta_surface
    remover.SizeFactor = 0.15
    remover.Execute()

    assert compare_surfaces(remover.Surface, name) == True
