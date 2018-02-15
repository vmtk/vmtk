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
import vmtk.vmtksurfaceappend as surfaceappend

def test_append_defaults(aorta_surface, aorta_surface2, compare_surfaces):
    name = __name__ + '_test_append_defaults.vtp'
    appender = surfaceappend.vmtkSurfaceAppend()
    appender.Surface = aorta_surface
    appender.Surface2 = aorta_surface2
    appender.Execute()

    assert compare_surfaces(appender.Surface, name)