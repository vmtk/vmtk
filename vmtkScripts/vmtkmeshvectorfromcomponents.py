#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshvectorfromcomponents.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkMeshVectorFromComponents(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.VectorArrayName = None
        self.ComponentsArrayNames = None

        self.RemoveComponentArrays = False

        self.SetScriptName('vmtkmeshvectorfromcomponents')
        self.SetScriptDoc('create a vector array from a number of scalar arrays treated as vector components')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['VectorArrayName','vector','str',1,'',''],
            ['ComponentsArrayNames','components','str',-1,'',''],
            ['RemoveComponentArrays','removecomponents','bool',1,'','']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        numberOfComponents = len(self.ComponentsArrayNames)

        vectorArray = vtk.vtkDoubleArray()
        vectorArray.SetName(self.VectorArrayName)
        vectorArray.SetNumberOfComponents(numberOfComponents)
        vectorArray.SetNumberOfTuples(self.Mesh.GetNumberOfPoints())

        for i in range(numberOfComponents):
            componentArray = self.Mesh.GetPointData().GetArray(self.ComponentsArrayNames[i])
            vectorArray.CopyComponent(i,componentArray,0)
            if self.RemoveComponentArrays:
                self.Mesh.GetPointData().RemoveArray(self.ComponentsArrayNames[i])

        self.Mesh.GetPointData().AddArray(vectorArray)


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
