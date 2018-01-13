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
import vmtk.vmtksurfacenormals as normals

def test_default_params(aorta_surface, compare_surfaces):
    name = __name__ + '_test_default_params.vtp'
    normer = normals.vmtkSurfaceNormals()
    normer.Surface = aorta_surface
    normer.Execute()

    assert compare_surfaces(normer.Surface, name, method='addpointarray', arrayname='Normals') == True


def test_no_autoorient_normals(aorta_surface, compare_surfaces):
    name = __name__ + '_test_no_autoorient_normals.vtp'
    normer = normals.vmtkSurfaceNormals()
    normer.Surface = aorta_surface
    normer.AutoOrientNormals = 0
    normer.Execute()

    assert compare_surfaces(normer.Surface, name, method='addpointarray', arrayname='Normals') == True


def test_no_consistency(aorta_surface, compare_surfaces):
    name = __name__ + '_test_no_consistency.vtp'
    normer = normals.vmtkSurfaceNormals()
    normer.Surface = aorta_surface
    normer.Consistency = 0
    normer.Execute()

    assert compare_surfaces(normer.Surface, name, method='addpointarray', arrayname='Normals') == True