#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacebooleanOperation.py,v $
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


class vmtkSurfaceBooleanOperation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Surface2 = None

        self.Tolerance = 1E-6
        self.Operation = 'union'
        self.Method = 'default'

        self.SetScriptName('vmtksurfacebooleanoperation')
        self.SetScriptDoc('perform a boolean operation between two surfaces')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Surface2','i2','vtkPolyData',1,'','the second input surface','vmtksurfacereader'],
            ['Tolerance','tolerance','float',1,'(0.0,)','tolerance for considering two points coincident'],
            ['Operation','operation','str',1,'["union","intersection","difference"]','the boolean operation to be performed'],
            ['Method','method','str',1,'["default","loop"]','method used for boolean operation']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        if self.Surface2 == None:
            self.PrintError('Error: No Surface2.')

        if not self.Method:
            self.PrintError('Error: No boolean method provided.')
        else:
            if self.Method == 'default':
                booleanOperationFilter = vtk.vtkBooleanOperationPolyDataFilter()
            elif self.Method == 'loop':
                booleanOperationFilter = vtk.vtkLoopBooleanPolyDataFilter()
            else:
                self.PrintError('Error: Method {0} not implemented.'.format(self.Method))

        booleanOperationFilter.SetInputData(0,self.Surface)
        booleanOperationFilter.SetInputData(1,self.Surface2)
        if self.Operation == 'union':
            booleanOperationFilter.SetOperationToUnion()
        elif self.Operation == 'intersection':
            booleanOperationFilter.SetOperationToIntersection()
        elif self.Operation == 'difference':
            booleanOperationFilter.SetOperationToDifference()
        booleanOperationFilter.SetTolerance(self.Tolerance)
        booleanOperationFilter.Update()

        self.Surface = booleanOperationFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
