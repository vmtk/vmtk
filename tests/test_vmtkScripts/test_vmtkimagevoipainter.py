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
import vmtk.vmtkimageviewer as imageviewer

#TODO: Figure out why this fails.
@pytest.mark.skip(reason='this currently does not work. getting error "BoxExtent exceeds input WholeExtent"')
def test_voi_painter(aorta_image, compare_images):
    name = __name__ + '_test_voi_painter.mha'
    painter = voipainter.vmtkImageVOIPainter()
    painter.Image = aorta_image
    painter.Interactive = 0
    painter.PaintValue = 1500.0
    painter.BoxBounds = [190.0, 259.0, 110.0, 283.0, 12.0, 37.0]
    painter.Execute()

    print(painter.BoxBounds)
    viewer = imageviewer.vmtkImageViewer()
    viewer.Image = painter.Image
    viewer.Execute()
    # assert compare_images(painter.Image, name) == True