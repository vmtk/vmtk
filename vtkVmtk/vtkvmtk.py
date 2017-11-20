#!/usr/bin/env python
""" This module loads all the classes from the vtkVmtk library into its
namespace.  This is a required module."""

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import os
import vtk

from .vtkvmtkCommonPython import *
from .vtkvmtkComputationalGeometryPython import *
from .vtkvmtkDifferentialGeometryPython import *
from .vtkvmtkIOPython import *
from .vtkvmtkMiscPython import *
try:
  from .vtkvmtkRenderingPython import *
except:
  pass
from .vtkvmtkSegmentationPython import *
from .vtkvmtkITKPython import *
try:
  from .vtkvmtkContribPython import *
except:
  pass
