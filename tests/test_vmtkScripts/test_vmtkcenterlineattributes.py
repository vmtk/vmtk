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
import vmtk.vmtkcenterlineattributes as centerlineattributes

def test_abscissas(aorta_centerline, compare_centerlines):
    name = __name__ + '_test_abscissas.vtp'
    attributes = centerlineattributes.vmtkCenterlineAttributes()
    attributes.Centerlines = aorta_centerline
    attributes.Execute()

    assert compare_centerlines(attributes.Centerlines, name, method='addpointarray', arrayname='Abscissas') == True


def test_paralleltransportnormals(aorta_centerline, compare_centerlines):
    name = __name__ + '_tets_paralleltransportnormals.vtp'
    attributes = centerlineattributes.vmtkCenterlineAttributes()
    attributes.Centerlines = aorta_centerline
    attributes.Execute()

    assert compare_centerlines(attributes.Centerlines, name, method='addpointarray', arrayname='ParallelTransportNormals') == True