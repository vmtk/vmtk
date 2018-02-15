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
import vmtk.vmtkcenterlinegeometry as centerlinegeometry

def test_default_parameters(aorta_centerline, compare_centerlines):
    name = __name__ + '_test_default_parameters.vtp'
    geometry = centerlinegeometry.vmtkCenterlineGeometry()
    geometry.Centerlines = aorta_centerline
    geometry.Execute()

    assert compare_centerlines(geometry.Centerlines, name, method='addcellarray', arrayname='Tortuosity') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addpointarray', arrayname='Torsion') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addpointarray', arrayname='Curvature') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addcellarray', arrayname='Length') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addpointarray', arrayname='FrenetTangent') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addpointarray', arrayname='FrenetNormal') == True
    assert compare_centerlines(geometry.Centerlines, name, method='addpointarray', arrayname='FrenetBinormal') == True




