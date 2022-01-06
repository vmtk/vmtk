#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshappend.py,v $
## Language:  Python
## Date:      $Date: 2021/09/16 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkSurfaceCoords(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.SetScriptName('vmtksurfacecoords')
        self.SetScriptDoc('add coordinates as point data array on the surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def CreateCoords(self,data):
        coords = data.GetPoints().GetData()
        coords.SetName('coords')
        data.GetPointData().AddArray(coords)

        names = ['coordsX', 'coordsY', 'coordsZ']
        for j in range(3):
            scalarCoords = vtk.vtkDoubleArray()
            scalarCoords.SetNumberOfComponents(1)
            scalarCoords.SetNumberOfTuples(data.GetNumberOfPoints())
            scalarCoords.SetName(names[j])
            for i in range(data.GetNumberOfPoints()):
                scalarCoords.SetValue(i,coords.GetComponent(i,j))
            data.GetPointData().AddArray(scalarCoords)

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        self.CreateCoords(self.Surface)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
