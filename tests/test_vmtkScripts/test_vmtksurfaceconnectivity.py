## Program: VMTK
## Language:  Python
## Date:      January 12, 2018
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
import vmtk.vmtksurfaceconnectivity as connectivity
import os


@pytest.fixture(scope='module')
def aorta_surface_two_segments(input_datadir):
    import vmtk.vmtksurfacereader as surfacereader
    reader = surfacereader.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface-two-segments.vtp')
    reader.Execute()
    return reader.Surface


def test_extract_largest_surface(aorta_surface_two_segments, compare_surfaces):
    name = __name__ + '_test_extract_largest_surface.vtp'
    connectiv = connectivity.vmtkSurfaceConnectivity()
    connectiv.Surface = aorta_surface_two_segments
    connectiv.Method = 'largest'
    connectiv.CleanOutput = 1
    connectiv.Execute()

    assert compare_surfaces(connectiv.Surface, name) == True


def test_extract_closest_to_reference_surface(aorta_surface_two_segments, aorta_surface_reference, compare_surfaces):
    name = __name__ + '_test_extract_closest_to_reference_surface.vtp'
    connectiv = connectivity.vmtkSurfaceConnectivity()
    connectiv.Surface = aorta_surface_two_segments
    connectiv.Method = 'closest'
    connectiv.ReferenceSurface = aorta_surface_reference
    connectiv.Execute()

    assert compare_surfaces(connectiv.Surface, name) == True


def test_extract_closest_to_point(aorta_surface_two_segments, compare_surfaces):
    name = __name__ + '_test_extract_closest_to_point.vtp'
    connectiv = connectivity.vmtkSurfaceConnectivity()
    connectiv.Surface = aorta_surface_two_segments
    connectiv.Method = 'closest'
    connectiv.ClosestPoint = [0.0, 0.0, 0.0]
    connectiv.Execute()

    assert compare_surfaces(connectiv.Surface, name) == True
