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

def test_normalize_image(aorta_image, image_to_sha):
    normer = norm.vmtkImageNormalize()
    normer.Image = aorta_image
    normer.Execute()

    assert image_to_sha(normer.Image) == '0e25a160968487bf9dc9a7217fe9fdb43a96d6f9'