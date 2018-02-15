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
import vmtk.vmtkimageotsuthresholds as otsu


@pytest.mark.parametrize("thresholds,paramid", [
    (1, '0'),
    (2, '1'),
    (3, '2'),
])
def test_otsu_image_multiple_thresholds(aorta_image, compare_images, thresholds, paramid):
    name = __name__ + '_test_otsu_image_multiple_thresholds_' + paramid + '.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfThresholds = thresholds
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True


def test_otsu_image_256_histogram_bins(aorta_image, compare_images):
    name = __name__ + '_test_otsu_image_256_histogram_bins.mha'
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfHistogramBins = 256
    otsuer.Execute()

    assert compare_images(otsuer.Image, name) == True