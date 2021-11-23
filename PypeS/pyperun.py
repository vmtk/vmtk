#!${PYTHON_SHEBANG}

## Program:   PypeS
## Module:    $RCSfile: pype.py,v $
## Language:  Python
## Date:      $Date: 2006/07/07 10:45:42 $
## Version:   $Revision: 1.18 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import sys
from multiprocessing import Process, Manager
try:
    from vmtk import pypeserver
    from vmtk import pypes
except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

if __name__=='__main__':
    manager = Manager()
    queue = manager.list()

    if sys.platform != 'darwin':
        pypeProcess = Process(target=pypeserver.PypeServer, args=(queue,None,None), kwargs={"returnIfEmptyQueue":True})
        pypeProcess.start()

    args = sys.argv[:]
    if sys.argv[0].startswith('pyperun'):
        args = sys.argv[1:]

    queue.append(args)

    try:
        if sys.platform != 'darwin':
            pypeProcess.join()
        else:
            pypeserver.PypeServer(queue,None,None,returnIfEmptyQueue=True)
    except KeyboardInterrupt:
        pypeProcess.terminate()
    except BaseException as e:
        print(str(e))
        sys.exit(1)
