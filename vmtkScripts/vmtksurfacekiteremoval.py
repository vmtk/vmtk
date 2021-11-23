#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacekiteremoval.py,v $
## Language:  Python
## Date:      $Date: 2006/03/01 11:53:48 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk
from vmtk import vtkvmtk
from vmtk import pypes


class vmtkSurfaceKiteRemoval(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.SizeFactor = 0.1

        self.SetScriptName('vmtksurfacekiteremoval')
        self.SetScriptDoc('remove small kites in a surface mesh to avoid Taubin smoothing artifacts')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['SizeFactor','sizefactor','float',1,'(0.0,)','ratio between local and global average triangle area']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        kiteRemoval = vtkvmtk.vtkvmtkPolyDataKiteRemovalFilter()
        kiteRemoval.SetInputData(self.Surface)
        kiteRemoval.SetSizeFactor(self.SizeFactor)
        kiteRemoval.Update()

        self.Surface = kiteRemoval.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
