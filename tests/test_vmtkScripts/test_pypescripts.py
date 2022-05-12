## Program: VMTK
## Language:  Python
## Date:      February 12, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Ramtin Gharleghi (Github @ramtingh)
##       University of New South Wales

import pytest
import inspect
from vmtk import vmtkscripts, pypes

scripts = []
for script in vmtkscripts.__dict__.values():
    if inspect.isclass(script) and issubclass(script, pypes.pypeScript):
        scripts.append(script)

@pytest.mark.parametrize("script",scripts)
def test_vmtkpypescript(script):
    obj = script()
    missing = []
    for member in obj.InputMembers:
        if not hasattr(obj, member.MemberName):
            missing.append(member.MemberName)
    assert not missing
