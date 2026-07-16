"""vmtk - the Vascular Modeling Toolkit.

This module is installed as vmtk/__init__.py in the Python wheels built by
the top-level setup.py (see the VMTK_WHEEL_BUILD option in the top-level
CMakeLists.txt). Regular (non-wheel) installs use the empty __init__.py.in
instead.

In the wheel layout the vmtk package directory contains both the wrapped
Python extension modules and the vmtk shared libraries, while the VTK
libraries live in the sibling "vtkmodules" package of the "vtk" wheel.
Since Python 3.8, Windows extension modules resolve dependent DLLs only
from directories registered with os.add_dll_directory(), so register both
directories here before any wrapped module is imported. Linux and macOS
need no equivalent: the libraries are built with relative rpaths
($ORIGIN / @loader_path) that resolve the same locations.
"""

import os as _os
import sys as _sys

if _sys.platform == "win32" and hasattr(_os, "add_dll_directory"):
    _package_dir = _os.path.dirname(_os.path.abspath(__file__))
    _vtkmodules_dir = _os.path.join(_os.path.dirname(_package_dir), "vtkmodules")
    for _dll_dir in (_package_dir, _vtkmodules_dir):
        if _os.path.isdir(_dll_dir):
            _os.add_dll_directory(_dll_dir)
    del _package_dir, _vtkmodules_dir, _dll_dir

del _os, _sys
