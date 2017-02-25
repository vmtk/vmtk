#!/usr/bin/env python

import importlib

__all__ = [
    'pypescript',
    'pype',
    'pypebatch'
    ]

globalList = globals()

for item in __all__:
    moduleToImport = 'vmtk.'+item
    globalList[moduleToImport] = importlib.import_module(moduleToImport)
