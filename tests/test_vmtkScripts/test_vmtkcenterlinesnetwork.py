## Program: VMTK
## Language:  Python
## Date:      May 30, 2018
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
import vmtk.vmtkcenterlinesnetwork as centerlinesnetwork


def test_centerline_extraction_surface_with_no_hole(aorta_surface):
    clnetwork = centerlinesnetwork.vmtkCenterlinesNetwork()
    clnetwork.Surface = aorta_surface
    clnetwork.RandomSeed = 42
    clnetwork.Execute()
    centerlines = clnetwork.Centerlines
    assert (centerlines.GetNumberOfPoints() > 100) == True
