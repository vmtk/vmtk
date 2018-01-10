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

def test_dilate_grayscale_image(aorta_image, image_to_sha):
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'dilate'
    morpher.Execute()

    assert image_to_sha(morpher.Image) == '489fc2a5f372426e6c2cd4d29a95c81383e9bbcc'


def test_erode_grayscale_image(aorta_image, image_to_sha):
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'erode'
    morpher.Execute()

    assert image_to_sha(morpher.Image) == 'eacee45a6e32c1cef043644cafbd5f823c3ef5d9'


def test_open_grayscale_image(aorta_image, image_to_sha):
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'open'
    morpher.Execute()

    assert image_to_sha(morpher.Image) == '83419dc74e3785f4c9689ef5d0c404fca035fd2b'


def test_close_grayscale_image(aorta_image, image_to_sha):
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'close'
    morpher.Execute()

    assert image_to_sha(morpher.Image) == '56c558031c5a5251dd2aa5d8d1503736d68252f5'


def test_change_ball_radius_dilate_grayscale_image(aorta_image, image_to_sha):
    morpher = morph.vmtkImageMorphology()
    morpher.Image = aorta_image
    morpher.Operation = 'dilate'
    morpher.BallRadius = [2, 2, 2]
    morpher.Execute()

    assert image_to_sha(morpher.Image) == 'e2be7af55aec94a544380804771bc2b9feca75b0'