#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetransformtoras.py,v $
## Language:  Python
## Date:      $Date: Sun Feb 21 17:02:37 CET 2010$
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceTransformToRAS(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.XyzToRasMatrixCoefficients = None
        self.InvertMatrix = 0

        self.SetScriptName('vmtksurfacetransformtoras')
        self.SetScriptDoc('transform a surface generated in XYZ image space into RAS space')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['XyzToRasMatrixCoefficients','matrix','float',16,'','coefficients of XYZToRAS transform matrix'],
            ['InvertMatrix','invert','bool',1,'','invert XyzToRasMatrixCoefficients matrix, additional transforms to surface in RAS space requires the inverse transform reverting the surface back to an axis aligned frame']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.XyzToRasMatrixCoefficients == None:
            self.PrintError('Error: no XyzToRasMatrixCoefficients.')

        matrix = vtk.vtkMatrix4x4()
        matrix.DeepCopy(self.XyzToRasMatrixCoefficients)

        if self.InvertMatrix:
            matrix.Invert()

        transform = vtk.vtkTransform()
        transform.SetMatrix(matrix)

        transformFilter = vtk.vtkTransformPolyDataFilter()
        transformFilter.SetInputData(self.Surface)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Surface = transformFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
