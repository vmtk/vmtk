## Module:    $RCSfile: vmtknumpywriter.py,v $
## Language:  Python
## Date:      June 10, 2017
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo
##       The Jacobs Institute

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes
import pickle

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtknumpytosurface module, numpy is not installed')

class vmtkNumpyWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.InputDict = None
        self.OutputFileName = ''

        self.SetScriptName('vmtkNumpyWriter')
        self.SetScriptDoc('Writes a dictionary containing numpy array data to a file')

        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input dictionary'],
            ['OutputFileName','ofile','str',1,'','the output file name']])
        self.SetOutputMembers([])

    def Execute(self):

        if self.ArrayDict == None:
            self.PrintError('Error: no input dictionary')

        if self.OutputFileName == '':
            self.PrintError('Error: no output file name specified')

        if self.OutputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputFileName = tkinter.filedialog.asksaveasfilename(title="Output Dictionary",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.OutputFileName)
            if not self.OutputFileName:
                self.PrintError('Error: no Output File Name.')

        self.PrintLog('Writing File')
        with open(self.OutputFileName, 'wb') as outfile:
            pickle.dump(self.ArrayDict, outfile, protocol=pickle.HIGHEST_PROTOCOL)

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()