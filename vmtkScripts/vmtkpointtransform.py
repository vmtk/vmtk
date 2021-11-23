#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkpointtransform.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkPointTransform(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Points = []

        self.MatrixCoefficients = []
        self.InvertMatrix = 0

        self.SetScriptName('vmtkpointtransform')
        self.SetScriptDoc('transform points with a provided matrix')
        self.SetInputMembers([
            ['Points','i','float',-1,'','point coordinates'],
            ['MatrixCoefficients','matrix','float',16,'','coefficients of transform matrix'],
            ['InvertMatrix','invert','bool',1,'','invert matrix before applying transformation']
            ])
        self.SetOutputMembers([
            ['Points','o','float',-1,'','output point coordinates']
            ])

    def Execute(self):

        if len(self.Points) % 3 != 0:
            self.PrintError('Error: Points not made up of triplets.')
            return

        matrix = vtk.vtkMatrix4x4()
        matrix.DeepCopy(self.MatrixCoefficients)
        if self.InvertMatrix:
            matrix.Invert()

        transform = vtk.vtkMatrixToLinearTransform()
        transform.SetInput(matrix)

        outputPoints = []
        for i in range(len(self.Points)//3):
            point = [self.Points[3*i+0],self.Points[3*i+1],self.Points[3*i+2]]
            outputPoint = transform.TransformPoint(point)
            outputPoints.append(outputPoint)

        self.Points = outputPoints


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
