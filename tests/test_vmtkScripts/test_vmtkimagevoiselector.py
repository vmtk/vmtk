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
import vmtk.vmtkimagevoiselector as voiselector

#TODO: Why does this fail, but the sha method passes?
@pytest.mark.skip(reason='This assertion fails for an unknown reason')
def test_voi_selector(aorta_image, compare_images):
    name = __name__ + '_test_voi_selector.mha'
    selector = voiselector.vmtkImageVOISelector()
    selector.Image = aorta_image
    selector.Interactive = 0
    selector.BoxBounds = [157.0, 186.0, 31.0, 67.0, 10.0, 20.0]
    selector.Execute()

    assert compare_images(selector.Image, name) == True

def test_voi_selector(aorta_image, image_to_sha):
    selector = voiselector.vmtkImageVOISelector()
    selector.Image = aorta_image
    selector.Interactive = 0
    selector.BoxBounds = [157.0, 186.0, 31.0, 67.0, 10.0, 20.0]
    selector.Execute()

    assert image_to_sha(selector.Image) == 'f834f6ba5a9e8caf86fb9e0f951650e5adc627cc'
