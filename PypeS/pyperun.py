#!/usr/bin/env python

## Program:   PypeS
## Module:    $RCSfile: pype.py,v $
## Language:  Python
## Date:      $Date: 2006/07/07 10:45:42 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
try:
    from vmtk import pypes
except:

    import os

    currentEnviron = {"PATH":"", "LD_LIBRARY_PATH":"", "DYLD_LIBRARY_PATH":"", "PYTHONPATH":""}

    if os.environ.has_key("PATH"):
        currentEnviron["PATH"] = os.environ["PATH"]

    if os.environ.has_key("LD_LIBRARY_PATH"):
        currentEnviron["LD_LIBRARY_PATH"] = os.environ["LD_LIBRARY_PATH"]

    if os.environ.has_key("DYLD_LIBRARY_PATH"):
        currentEnviron["DYLD_LIBRARY_PATH"] = os.environ["DYLD_LIBRARY_PATH"]

    if os.environ.has_key("PYTHONPATH"):
        currentEnviron["PYTHONPATH"] = os.environ["PYTHONPATH"]

    newEnviron = {}

    newEnviron["PATH"] = os.path.join(vmtkhome,'bin')

    vtkdir = [el for el in os.listdir(os.path.join(vmtkhome,"lib")) if el.startswith('vtk')][0]

    newEnviron["LD_LIBRARY_PATH"] = os.path.join(vmtkhome,"lib",vtkdir) + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib","vmtk") + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib","InsightToolkit")

    newEnviron["DYLD_LIBRARY_PATH"] = os.path.join(vmtkhome,"lib",vtkdir) + os.path.pathsep + \
                                      os.path.join(vmtkhome,"lib","vmtk") + os.path.pathsep + \
                                      os.path.join(vmtkhome,"lib","InsightToolkit")

    newEnviron["PYTHONPATH"] =  os.path.join(vmtkhome,"bin","Python") + os.path.pathsep + \
                                os.path.join(vmtkhome,"lib",vtkdir) + os.path.pathsep + \
                                os.path.join(vmtkhome,"lib","vmtk")

    os.environ["PATH"] = newEnviron["PATH"] + os.path.pathsep + currentEnviron["PATH"]
    os.environ["LD_LIBRARY_PATH"] = newEnviron["LD_LIBRARY_PATH"] + os.path.pathsep + currentEnviron["LD_LIBRARY_PATH"]
    os.environ["DYLD_LIBRARY_PATH"] = newEnviron["DYLD_LIBRARY_PATH"] + os.path.pathsep + currentEnviron["DYLD_LIBRARY_PATH"]
    os.environ["PYTHONPATH"] = newEnviron["PYTHONPATH"] + os.path.pathsep + currentEnviron["PYTHONPATH"]

    from vmtk import pypes


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

