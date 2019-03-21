#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes

class vmtkSurfaceEdgeLengthArray(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        # self.CellEntityIdsArrayName = 'CellEntityIds'
        # self.CellEntityIdsArray = None
        self.InputArrayName = 'Thickness'
        self.OutputArrayName = 'EdgeLength'
        self.InputArray = None
        self.OutputArray = None
        self.MinSize = 0.0
        self.MaxSize = 1E16
        self.Alpha = 1.0
        self.Beta = 1.0

        self.SetScriptName('vmtksurfaceedgelengtharray')
        self.SetScriptDoc('given a scalar array f, compute the edge length array as max( minsize, min( alpha*f^beta, maxsize ) )')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            # ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the tags are stored'],
            ['InputArrayName','inputarray','str',1,'','name of the input array f'],
            ['OutputArrayName','outputarray','str',1,'','name of the array where the edge length is stored'],
            ['MinSize','minsize','float',1,'(0.0,)','minimum edge length'],
            ['MaxSize','maxsize','float',1,'(0.0,)','maximum edge length'],
            ['Alpha','alpha','float',1,'(0.0,)','multiplicative factor to the input array'],
            ['Beta','beta','float',1,'(0.0,)','exponent to the input array']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            # ['CellEntityIdsArray','oentityidsarray','vtkIntArray',1,'','the output entity ids array']
            ])


    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        self.InputArray = self.Surface.GetPointData().GetArray(self.InputArrayName)

        if self.InputArray == None:
            self.PrintError('Error: No PointData Array called '+self.ArrayName+' defined on the input Surface')

        self.OutputArray = vtk.vtkDoubleArray()
        self.OutputArray.SetName(self.OutputArrayName)
        self.OutputArray.SetNumberOfComponents(1)
        self.OutputArray.SetNumberOfTuples(self.Surface.GetNumberOfPoints())
        self.Surface.GetPointData().AddArray(self.OutputArray)

        for i in range(self.Surface.GetNumberOfPoints()):
            input = self.InputArray.GetComponent(i,0)
            value = max( self.MinSize, min( pow(self.Alpha*input,self.Beta), self.MaxSize ) )
            self.OutputArray.SetComponent(i,0,value)




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()