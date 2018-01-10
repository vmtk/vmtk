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


@pytest.mark.parametrize("thresholds,expected_hash", [
    (1, 'd692f244d88a2f4544fd111d26a337048568b75e'),
    (2, '4f3320f66ca6d50e53ef7da620bb3cc2c90e6069'),
    (3, '46381069b0def5db65f51a27568d0a6ed19ee790'),
])
def test_otsu_image_multiple_thresholds(aorta_image, image_to_sha, thresholds, expected_hash):
    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfThresholds = thresholds
    otsuer.Execute()

    assert image_to_sha(otsuer.Image) == expected_hash


def test_otsu_image_256_histogram_bins(aorta_image, image_to_sha):

    otsuer = otsu.vmtkImageOtsuThresholds()
    otsuer.Image = aorta_image
    otsuer.NumberOfHistogramBins = 256
    otsuer.Execute()

    assert image_to_sha(otsuer.Image) == '2666f107392cbc193d3c9cd1572637e2a1d17f6c'