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
import vmtk.vmtkimagecompose as comp

def test_multiply_images(aorta_image, compare_images):
    name = __name__ + '_test_multiply_images.mha'
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert compare_images(composer.Image, name) == True


def test_subtract_images(aorta_image, compare_images):
    name = __name__ + '_test_subtract_images.mha'
    composer = comp.vmtkImageCompose()
    composer.Operation = 'subtract'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert compare_images(composer.Image, name) == True


def test_negate_image2_and_multiply(aorta_image, compare_images):
    name = __name__ + '_test_negate_image2_and_multiply.mha'
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert compare_images(composer.Image, name) == True


def test_negate_image2_and_min(aorta_image, compare_images):
    name = __name__ + '_test_negate_image2_and_min.mha'
    composer = comp.vmtkImageCompose()
    composer.Operation = 'min'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert compare_images(composer.Image, name) == True


def test_negate_image2_and_max(aorta_image, compare_images):
    name = __name__ + '_test_negate_image2_and_max.mha'
    composer = comp.vmtkImageCompose()
    composer.Operation = 'max'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert compare_images(composer.Image, name) == True