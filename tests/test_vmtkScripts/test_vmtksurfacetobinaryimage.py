## Program: VMTK
## Language:  Python
## Date:      May 2, 2018
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
import vmtk.vmtksurfacetobinaryimage as surfacetobinaryimage

@pytest.mark.parametrize("insidevalue,outsidevalue,paramid", [
    (1, 0 ,'0'),
    (255, 0, '1'),
    (255, 1, '2'),
    (1, 255, '3'),
])
def test_set_inside_outside_values(aorta_surface, compare_images,
                                   insidevalue, outsidevalue, paramid, write_image):
    name = __name__ + '_test_set_inside_outside_values_' + paramid + '.vtp'
    surfacetobinaryimage.Surface = aorta_surface
    surfacetobinaryimage.InsideValue = insidevalue
    surfacetobinaryimage.OutsideValue = outsidevalue
    surfacetobinaryimage.Execute()
    write_image(surfacetobinaryimage.Image, name)
    
    assert compare_images(surfacetobinaryimage.Image, name) == True


def test_change_spacing(aorta_surface, compare_images, write_image):
    name = __name__ + '_test_change_spacing.vtp'
    surfacetobinaryimage.Surface = aorta_surface
    surfacetobinaryimage.PolyDataToImageDataSpacing = [0.4, 0.4, 0.4]
    surfacetobinaryimage.Execute()
    write_image(surfacetobinaryimage.Image, name)
    
    assert compare_images(surfacetobinaryimage.Image, name) == True