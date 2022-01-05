#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtonumpy.py,v $
## Language:  Python
## Date:      FEB 23, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkMeshInfo(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CellEntityIdsArrayName = None

        self.SetScriptName('vmtkMeshInfo')
        self.SetScriptDoc('Print mesh info about cell types')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1]
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        print('\nNumber of points:',self.Mesh.GetNumberOfPoints())
        print('\nNumber of cells:',self.Mesh.GetNumberOfCells())

        typeDict = {3:"lines",
                    4:"poly-lines",
                    5:"triangles",
                    9:"quads",
                    10:"tetrahedra",
                    12:"hexahedra"}

        for i,item in enumerate(typeDict):
            idArray = vtk.vtkIdTypeArray()
            self.Mesh.GetIdsOfCellsOfType(item,idArray)
            if idArray.GetNumberOfTuples() > 0:
                print(' - number of',typeDict[item],idArray.GetNumberOfTuples())

        print('\n')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
