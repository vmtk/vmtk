## Program: VMTK
## Language:  Python
## Date:      January 24, 2018
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
import vmtk.vmtkcenterlines as vmtkcenterlines
import vmtk.vmtkcenterlineviewer as viewer

def test_idlist_centerlines(aorta_surface, compare_centerlines):
    name = __name__ + '_test_idlist_centerlines.vtp'
    centerliner = vmtkcenterlines.vmtkCenterlines()
    centerliner.Surface = aorta_surface
    centerliner.SeedSelectorName = 'idlist'
    centerliner.SourceIds = [2334]
    centerliner.TargetIds = [5561, 6131]
    centerliner.Execute()

    assert compare_centerlines(centerliner.Centerlines, name) == True


def test_carotidprofiles_centerlines(aorta_surface_openends, compare_centerlines):
    name = __name__ + '_test_carotidprofiles_centerlines.vtp'
    centerliner = vmtkcenterlines.vmtkCenterlines()
    centerliner.Surface = aorta_surface_openends
    centerliner.SeedSelectorName = 'carotidprofiles'
    centerliner.Execute()

    assert compare_centerlines(centerliner.Centerlines, name) == True


def test_profileidlist_centerlines(aorta_surface_openends, compare_centerlines):
    name = __name__ + '_test_profileidlist_centerlines.vtp'
    centerliner = vmtkcenterlines.vmtkCenterlines()
    centerliner.Surface = aorta_surface_openends
    centerliner.SeedSelectorName = 'profileidlist'
    centerliner.SourceIds = [0]
    centerliner.TargetIds = [1, 2]
    centerliner.Execute()

    assert compare_centerlines(centerliner.Centerlines, name) == True