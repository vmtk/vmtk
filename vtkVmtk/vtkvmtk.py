""" This module loads all the classes from the vtkVmtk library into its
namespace.  This is a required module."""

import os
import vtk

from vtkvmtkCommonPython import *
from vtkvmtkComputationalGeometryPython import *
from vtkvmtkDifferentialGeometryPython import *
from vtkvmtkIOPython import *
from vtkvmtkMiscPython import *
try:
  from vtkvmtkRenderingPython import *
except:
  pass
from vtkvmtkSegmentationPython import *
from vtkvmtkITKPython import *
try:
  from vtkvmtkContribPython import *
except:
  pass
