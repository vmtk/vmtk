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
import vmtk.vmtkcenterlineinterpolation as centerlineinterpolation


def test_default_params_MaximumInscribedSphereRadius(aorta_centerline, compare_centerlines):
    name = __name__ + "_test_default_params_MaximumInscribedSphereRadius.vtp"
    interpolator = centerlineinterpolation.vmtkCenterlineInterpolation()
    interpolator.Centerlines = aorta_centerline
    interpolator.MaskArrayName = "MaximumInscribedSphereRadius"
    interpolator.Execute()

    assert compare_centerlines(interpolator.Centerlines, name) == True


def test_default_params_EdgeArray(aorta_centerline, compare_centerlines):
    name = __name__ + "_test_default_params_EdgeArray.vtp"
    interpolator = centerlineinterpolation.vmtkCenterlineInterpolation()
    interpolator.Centerlines = aorta_centerline
    interpolator.MaskArrayName = "EdgeArray"
    interpolator.Execute()

    assert compare_centerlines(interpolator.Centerlines, name) == True


def test_default_params_EdgePCoordArray(aorta_centerline, compare_centerlines):
    name = __name__ + "_test_default_params_EdgePCoordArray.vtp"
    interpolator = centerlineinterpolation.vmtkCenterlineInterpolation()
    interpolator.Centerlines = aorta_centerline
    interpolator.MaskArrayName = "EdgePCoordArray"
    interpolator.Execute()

    assert compare_centerlines(interpolator.Centerlines, name) == True
