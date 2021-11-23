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
import os
from hashlib import sha1
import vmtk.vmtksurfacetonumpy as wrap
import vmtk.vmtksurfacereader as r


def test_read_vtp_surface(input_datadir):
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'aorta-surface.vtp')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == '2d589cf877f713cf805d7be952fa5bdb2f2ef0ee'


def test_read_stl_surface(input_datadir):
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'fixture.stl')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == 'a3b3c7608de5a7a9fecf9f7280f9fc65565e7ded'
