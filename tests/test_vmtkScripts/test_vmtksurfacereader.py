import pytest
import os
from hashlib import sha1
import vmtk.vmtksurfacetonumpy as wrap

def test_read_vtp_surface(test_data):
    import vmtk.vmtksurfacereader as r
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(test_data, 'cow.vtp')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == 'f8f8bdc4e08a266cf6418f51e805aa689dc2f448'

def test_read_stl_surface(test_data):
    import vmtk.vmtksurfacereader as r
    reader = r.vmtkSurfaceReader()
    reader.InputFileName = os.path.join(test_data, 'fixture.stl')
    reader.Execute()

    conv = wrap.vmtkSurfaceToNumpy()
    conv.Surface = reader.Surface
    conv.Execute()

    assert sha1(conv.ArrayDict['Points']).hexdigest() == 'a3b3c7608de5a7a9fecf9f7280f9fc65565e7ded'

