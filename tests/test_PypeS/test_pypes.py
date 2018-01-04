#!/usr/bin/env python

## Program:   VMTK Tests
## Module:    $RCSfile: test_pypes.py,v $
## Language:  Python
## Date:      $Date: 12 DEC 2018$
## Version:   $Revision: 0.1 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import unittest

def test_import_pypes():
    from vmtk import pypes
    assert pypes.Pype

