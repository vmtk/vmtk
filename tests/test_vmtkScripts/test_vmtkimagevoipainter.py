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
import vmtk.vmtkimagevoipainter as voipainter

#TODO: Figure out why this fails.
@pytest.mark.skip(reason='this currently does not work. getting error "BoxExtent exceeds input WholeExtent"')
def test_voi_painter(aorta_image, image_to_sha):
    painter = voipainter.vmtkImageVOIPainter()
    painter.Image = aorta_image
    painter.Interactive = 0
    painter.BoxBounds = [157.0, 186.0, 31.0, 67.0, 10.0, 20.0]
    painter.Execute()

    print(image_to_sha(painter.Image))