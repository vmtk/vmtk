## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import vmtk.vmtksurfacemassproperties as massproperties


def test_surface_mass_properties(aorta_surface):
    props = massproperties.vmtkSurfaceMassProperties()
    props.Surface = aorta_surface
    props.Execute()

    assert props.SurfaceArea == pytest.approx(4517.763081539069, 0.5)
    assert props.Volume == pytest.approx(13184.266682666812, 0.5)
    assert props.ShapeIndex == pytest.approx(1.29380156100349, 0.2)
