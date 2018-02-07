## Program: VMTK
## Language:  Python
## Date:      February 12, 2018
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
import vmtk.vmtkcenterlinesmoothing as centerlinesmoothing
from vtk.numpy_interface import dataset_adapter as dsa 
import numpy as np

def test_default_params(aorta_centerline, compare_centerlines):
    name = __name__ + '_test_default_params.vtp'
    smoother = centerlinesmoothing.vmtkCenterlineSmoothing()
    smoother.Centerlines = aorta_centerline
    smoother.Execute()

    assert compare_centerlines(smoother.Centerlines, name) == True

@pytest.mark.parametrize("iterations,factor,paramid",[
    (100, 0.2, '0'),
    (200, 0.1, '1'),
    (200, 0.2, '2'),
    (50, 0.5, '3')
])
def test_varied_parameters(aorta_centerline, compare_centerlines,
                        iterations, factor, paramid):
    name = __name__ + '_test_varied_parameters_' + paramid + '.vtp'
    smoother = centerlinesmoothing.vmtkCenterlineSmoothing()
    smoother.Centerlines = aorta_centerline
    smoother.NumberOfSmoothingIterations = iterations
    smoother.SmoothingFactor = factor
    smoother.Execute()

    assert compare_centerlines(smoother.Centerlines, name) == True