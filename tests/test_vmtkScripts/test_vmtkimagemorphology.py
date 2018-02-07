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
import vmtk.vmtkimagemorphology as morph

def test_dilate_grayscale_image(aorta_image, compare_images):
    name = __name__ + '_test_dilate_grayscale_image.mha'
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'dilate'
    morpher.Execute()

    assert compare_images(morpher.Image, name) == True


def test_erode_grayscale_image(aorta_image, compare_images):
    name = __name__ + '_test_erode_grayscale_image.mha'
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'erode'
    morpher.Execute()

    assert compare_images(morpher.Image, name) == True


def test_open_grayscale_image(aorta_image, compare_images):
    name = __name__ + '_test_open_grayscale_image.mha'
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'open'
    morpher.Execute()

    assert compare_images(morpher.Image, name) == True


def test_close_grayscale_image(aorta_image, compare_images):
    name = __name__ + '_test_close_grayscale_image.mha'
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'close'
    morpher.Execute()

    assert compare_images(morpher.Image, name) == True


def test_change_ball_radius_dilate_grayscale_image(aorta_image, compare_images):
    name = __name__ + '_test_change_ball_radius_dilate_grayscale_image.mha'
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'dilate'
    morpher.BallRadius = [2, 2, 2]
    morpher.Execute()

    assert compare_images(morpher.Image, name) == True