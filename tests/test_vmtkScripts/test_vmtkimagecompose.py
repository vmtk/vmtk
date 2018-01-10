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

def test_multiply_images(aorta_image, image_to_sha):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert image_to_sha(composer.Image) == '9b87984045e3756840562fe06fbf88e63be3c7d3'


def test_subtract_images(aorta_image, image_to_sha):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'subtract'
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert image_to_sha(composer.Image) == '2d717c12ea94d12d75c2b2412661cbb1e193c5e2'


def test_negate_image2_and_multiply(aorta_image, image_to_sha):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'multiply'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert image_to_sha(composer.Image) == '002b8f78b7da8375bc31d762edcba8867bcb79bf'


def test_negate_image2_and_min(aorta_image, image_to_sha):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'min'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert image_to_sha(composer.Image) == 'be29a3a239cc798c3de812921f2d3a9c82b9cd2f'


def test_negate_image2_and_max(aorta_image, image_to_sha):
    composer = comp.vmtkImageCompose()
    composer.Operation = 'max'
    composer.NegateImage2 = True
    composer.Image = aorta_image
    composer.Image2 = aorta_image
    composer.Execute()

    assert image_to_sha(composer.Image) == 'd84290a3d556f5eb7c5e25a8075c9078791ec57d'