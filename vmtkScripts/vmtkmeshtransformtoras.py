#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtransformtoras.py,v $
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


class vmtkMeshTransformToRAS(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.XyzToRasMatrixCoefficients = None

        self.SetScriptName('vmtkmeshtransformtoras')
        self.SetScriptDoc('transform a mesh generated in XYZ image space into RAS space')
        self.SetInputMembers([
            ['Mesh','i','vtkPolyData',1,'','the input mesh','vmtkmeshreader'],
            ['XyzToRasMatrixCoefficients','matrix','float',16,'','coefficients of XYZToRAS transform matrix']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkPolyData',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: no Mesh.')

        if self.XyzToRasMatrixCoefficients == None:
            self.PrintError('Error: no XyzToRasMatrixCoefficients.')

        matrix = vtk.vtkMatrix4x4()
        matrix.DeepCopy(self.XyzToRasMatrixCoefficients)

        transform = vtk.vtkTransform()
        transform.SetMatrix(matrix)

        transformFilter = vtk.vtkTransformFilter()
        transformFilter.SetInputData(self.Mesh)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Mesh = transformFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
