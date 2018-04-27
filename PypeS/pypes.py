#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import importlib

__all__ = [
    'vmtk.pypescript',
    'vmtk.pype',
    'vmtk.pypebatch'
    ]

for item in __all__:
    globals().update(importlib.import_module(item).__dict__) 