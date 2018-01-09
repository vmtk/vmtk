""" This module loads all the classes from the contrib library into its
namespace.  This is a required module."""

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import os
import vtk
from vmtk import vtkvmtk

if os.name == 'posix':
    from .libvtkvmtkContribPython import *
else:
    from .vtkvmtkContribPython import *
    
