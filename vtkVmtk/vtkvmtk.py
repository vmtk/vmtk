""" This module loads all the classes from the vtkVmtk library into its
namespace.  This is a required module."""

import os
import vtk

if os.name == 'posix':
    from libvtkvmtkCommonPython import *
    from libvtkvmtkComputationalGeometryPython import *
    from libvtkvmtkDifferentialGeometryPython import *
    from libvtkvmtkIOPython import *
    from libvtkvmtkMiscPython import *
    from libvtkvmtkRenderingPython import *
    from libvtkvmtkSegmentationPython import *
    from libvtkvmtkITKPython import *
else:
    from vtkvmtkCommonPython import *
    from vtkvmtkComputationalGeometryPython import *
    from vtkvmtkDifferentialGeometryPython import *
    from vtkvmtkIOPython import *
    from vtkvmtkMiscPython import *
    from vtkvmtkRenderingPython import *
    from vtkvmtkSegmentationPython import *
    from vtkvmtkITKPython import *
    
