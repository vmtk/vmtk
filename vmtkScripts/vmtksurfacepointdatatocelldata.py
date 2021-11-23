#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacepointdatatocelldata.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkSurfacePointDataToCellData(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.SetScriptName('vmtksurfacepointdatatocelldata')
        self.SetScriptDoc('convert point data arrays to cell data surface arrays')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        pointDataToCellDataFilter = vtk.vtkPointDataToCellData()
        pointDataToCellDataFilter.SetInputData(self.Surface)
        pointDataToCellDataFilter.PassPointDataOn()
        pointDataToCellDataFilter.Update()

        self.Surface = pointDataToCellDataFilter.GetPolyDataOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
