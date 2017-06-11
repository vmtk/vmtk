## Module:    $RCSfile: vmtknumpyreader.py,v $
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

from vmtk import pypes
import pickle

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtknumpyreader module, numpy is not installed')

class vmtkNumpyReader(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.InputFileName = ''
        self.ArrayDict = None

        self.SetScriptName('vmtkNumpyReader')
        self.SetScriptDoc('reads a pickled file containing a numpy dictionary into an output array')

        self.SetInputMembers([
            ['InputFileName','i','str',1,'','the input file name']])
        self.SetOutputMembers([
            ['ArrayDict','ofile','dict',1,'','the output dictionary']
        ])

    def Execute(self):

        if self.InputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.InputFileName = tkinter.filedialog.askopenfilename(title="Input file",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.InputFileName)
            if not self.InputFileName:
                self.PrintError('Error: no InputFileName.')

        with open(self.InputFileName, 'rb') as infile:
            self.ArrayDict = pickle.load(infile)

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()