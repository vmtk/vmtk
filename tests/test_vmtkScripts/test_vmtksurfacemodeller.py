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
import vmtk.vmtksurfacemodeller as modeller


def test_default_params(aorta_surface, compare_images):
    name = __name__ + '_test_default_params.mha'
    model = modeller.vmtkSurfaceModeller()
    model.Surface = aorta_surface
    model.Execute()

    assert compare_images(model.Image, name) == True


def test_turn_off_negative_inside(aorta_surface, compare_images):
    name = __name__ + '_test_negative_inside.mha'
    model = modeller.vmtkSurfaceModeller()
    model.Surface = aorta_surface
    model.NegativeInside = 0
    model.Execute()

    assert compare_images(model.Image, name) == True


def test_change_sample_spacing(aorta_surface, compare_images):
    name = __name__ + '_test_change_sample_spacing.mha'
    model = modeller.vmtkSurfaceModeller()
    model.Surface = aorta_surface
    model.SampleSpacing = -0.5
    model.Execute()

    assert compare_images(model.Image, name) == True
