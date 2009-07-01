""" This module loads all the classes from the contrib library into its
namespace.  This is a required module."""

import os
import vtk
import vtkvmtk

if os.name == 'posix':
    from libvtkvmtkContribPython import *
else:
    from vtkvmtkContribPython import *
    
