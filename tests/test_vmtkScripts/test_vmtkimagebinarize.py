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
import vmtk.vmtkimagebinarize as binarize


def test_binarize_default_parameters(aorta_image, compare_images):
    name = __name__ + '_test_binarize_default_parameters.mha'
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Execute()
    
    assert compare_images(binarizer.Image, name) == True


def test_binarize_simple_threshold(aorta_image, compare_images):
    name = __name__ + '_test_binarize_simple_threshold.mha'
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.Execute()    

    assert compare_images(binarizer.Image, name) == True


def test_binarize_modify_lower_label(aorta_image, compare_images):
    name = __name__ + '_test_binarize_modify_lower_label.mha'
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.LowerLabel = -1
    binarizer.Execute()    

    assert compare_images(binarizer.Image, name) == True


def test_binarize_modify_upper_label(aorta_image, compare_images):
    name = __name__ + '_test_binarize_modify_upper_label.mha'
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.UpperLabel = 7
    binarizer.Execute()

    assert compare_images(binarizer.Image, name) == True


def test_binarize_modify_upper_and_lower_label(aorta_image, compare_images):
    name = __name__ + '_test_binarize_modify_upper_and_lower_label.mha'
    binarizer = binarize.vmtkImageBinarize()
    binarizer.Image = aorta_image
    binarizer.Threshold = 500
    binarizer.UpperLabel = 7
    binarizer.LowerLabel = 3
    binarizer.Execute()

    assert compare_images(binarizer.Image, name) == True