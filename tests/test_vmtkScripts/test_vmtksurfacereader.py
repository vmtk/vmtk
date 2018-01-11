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
import os
from hashlib import sha1
import vmtk.vmtksurfacetonumpy as wrap

def test_read_vtp_surface(input_datadir):
    import vmtk.vmtksurfacereader as r
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'cow.vtp')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == 'f8f8bdc4e08a266cf6418f51e805aa689dc2f448'

def test_read_stl_surface(input_datadir):
    import vmtk.vmtksurfacereader as r
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(input_datadir, 'fixture.stl')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == 'a3b3c7608de5a7a9fecf9f7280f9fc65565e7ded'

