#!/usr/bin/env python

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY

__all__ = [
    'vmtk.pypescript',
    'vmtk.pype',
    'vmtk.pypebatch'
    ]

for item in __all__:
    exec('from '+item+' import *')
