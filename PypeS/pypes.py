#!/usr/bin/env python

import importlib

__all__ = [
    'vmtk.pypescript',
    'vmtk.pype',
    'vmtk.pypebatch'
    ]

for item in __all__:
    exec('from '+item+' import *')
