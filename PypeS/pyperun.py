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
from multiprocessing import Process, Manager
try:
    from vmtk import pypeserver
    from vmtk import pypes
except:

    import os

    if sys.platform == 'darwin':
        ldEnvironmentVariable = "DYLD_LIBRARY_PATH"

    elif sys.platform == 'win32':
        ldEnvironmentVariable = "PATH"

    else:
        ldEnvironmentVariable = "LD_LIBRARY_PATH"

    currentEnviron = dict()
    currentEnviron[ldEnvironmentVariable] = ""
    currentEnviron["PYTHONPATH"] = ""

    if os.environ.has_key(ldEnvironmentVariable):
        currentEnviron[ldEnvironmentVariable] = os.environ[ldEnvironmentVariable]

    if os.environ.has_key("PYTHONPATH"):
        currentEnviron["PYTHONPATH"] = os.environ["PYTHONPATH"]

    newEnviron = {}

    vmtkhome = os.path.dirname(os.path.abspath(__file__))

    if vmtkhome.endswith('bin'):
        vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..")
    else:
        vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..","..","..")
	
    vtkdir = [el for el in os.listdir(os.path.join(vmtkhome,"lib")) if el.startswith('vtk')][0]

    newEnviron[ldEnvironmentVariable] = os.path.join(vmtkhome,"bin") + os.path.pathsep + \
	                                os.path.join(vmtkhome,"lib",vtkdir) + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib","vmtk") + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib","InsightToolkit")

    os.environ[ldEnvironmentVariable] = newEnviron[ldEnvironmentVariable] + os.path.pathsep + currentEnviron[ldEnvironmentVariable]

    sys.path.append(os.path.join(vmtkhome,"bin","Python"))
    sys.path.append(os.path.join(vmtkhome,"lib",vtkdir))
    sys.path.append(os.path.join(vmtkhome,"lib","vmtk"))

    from vmtk import pypeserver
    from vmtk import pypes


if __name__=='__main__':
    manager = Manager()
    queue = manager.list()
    pypeProcess = Process(target=pypeserver.PypeServer, args=(queue,None), kwargs={"returnIfEmptyQueue":True})
    pypeProcess.start()

    args = sys.argv[:]
    if sys.argv[0].startswith('pyperun'):
        args = sys.argv[1:]

    queue.append(' '.join(args))

    try:
        pypeProcess.join()
    except KeyboardInterrupt:
        pypeProcess.terminate()

    #main = pypes.pypeMain()
    #main.Arguments = sys.argv
    #main.Execute()

