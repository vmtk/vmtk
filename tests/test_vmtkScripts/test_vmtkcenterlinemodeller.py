## Program: VMTK
## Language:  Python
## Date:      February 2, 2018
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
import vmtk.vmtkcenterlinemodeller as centerlinemodeller

def test_default_params(aorta_centerline, compare_images):
    name = __name__ + '_test_default_params.vti'
    modeller = centerlinemodeller.vmtkCenterlineModeller()
    modeller.Centerlines = aorta_centerline
    modeller.RadiusArrayName = 'MaximumInscribedSphereRadius'
    modeller.Execute()

    assert compare_images(modeller.Image, name) == True


def test_128x_dimensions(aorta_centerline, compare_images):
    name = __name__ + '_test_48x_dimensions.vti'
    modeller = centerlinemodeller.vmtkCenterlineModeller()
    modeller.Centerlines = aorta_centerline
    modeller.RadiusArrayName = 'MaximumInscribedSphereRadius'
    modeller.SampleDimensions = [48, 48, 48]
    modeller.Execute()

    assert compare_images(modeller.Image, name) == True


def test_negate_function(aorta_centerline, compare_images):
    name = __name__ + '_test_negate_function.vti'
    modeller = centerlinemodeller.vmtkCenterlineModeller()
    modeller.Centerlines = aorta_centerline
    modeller.RadiusArrayName = 'MaximumInscribedSphereRadius'
    modeller.NegateFunction = 1
    modeller.Execute()

    assert compare_images(modeller.Image, name) == True


