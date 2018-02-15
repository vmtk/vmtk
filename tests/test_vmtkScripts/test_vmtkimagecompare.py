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
import os
from hashlib import sha1
import vmtk.vmtkimagetonumpy as wrap
import vmtk.vmtknumpytoimage as wrap2
import vmtk.vmtkimagecompare as comp
import vmtk.vmtkimagereader as r


def test_compare_same_image(aorta_image):

    comparer = comp.vmtkImageCompare()
    comparer.Method = 'subtraction'
    comparer.Image = aorta_image
    comparer.ReferenceImage = aorta_image
    comparer.Execute()

    assert comparer.Result is True


def test_compare_not_same_image(aorta_image):

    wraper = wrap.vmtkImageToNumpy()
    wraper.Image = aorta_image
    wraper.Execute()

    multiplied = wraper.ArrayDict
    multiplied['PointData']['ImageScalars'] = multiplied['PointData']['ImageScalars'] * 2

    back = wrap2.vmtkNumpyToImage()
    back.ArrayDict = multiplied
    back.Execute()

    comparer = comp.vmtkImageCompare()
    comparer.Method = 'subtraction'
    comparer.Image = aorta_image
    comparer.ReferenceImage = back.Image
    comparer.Execute()

    assert comparer.Result is False

#TODO: See why subtraction fails