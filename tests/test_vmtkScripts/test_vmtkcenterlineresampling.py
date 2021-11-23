## Program: VMTK
## Language:  Python
## Date:      February 2, 2018
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
import vmtk.vmtkcenterlineresampling as centerlineresampling


def test_default_params(aorta_centerline, compare_centerlines):
    name = __name__ + '_test_default_params.vtp'
    resampler = centerlineresampling.vmtkCenterlineResampling()
    resampler.Centerlines = aorta_centerline
    resampler.Length = 0.05
    resampler.Execute()

    assert resampler.Centerlines.GetNumberOfPoints() > 3000
    assert compare_centerlines(resampler.Centerlines, name) == True
