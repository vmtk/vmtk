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
import vmtk.vmtkimagenormalize as norm

def test_normalize_image(aorta_image, compare_images):
    name = __name__ + '_test_normalize_image.mha'
    normer = norm.vmtkImageNormalize()
    normer.Image = aorta_image
    normer.Execute()

    assert compare_images(normer.Image, name) == True