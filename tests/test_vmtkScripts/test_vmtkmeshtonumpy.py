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
import vmtk.vmtkmeshtonumpy as meshtonumpy

@pytest.fixture(scope='module')
def mesh_arraydict(aorta_mesh):
    converter = meshtonumpy.vmtkMeshToNumpy()
    converter.Mesh = aorta_mesh
    converter.Execute()
    return converter.ArrayDict

@pytest.fixture(scope='module')
def mesh_arraydict_nocell_strings(aorta_mesh):
    converter = meshtonumpy.vmtkMeshToNumpy()
    converter.Mesh = aorta_mesh
    converter.ReturnCellTypesAsStrings = 0
    converter.Execute()
    return converter.ArrayDict