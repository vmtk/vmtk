#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetransform.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import sys

import pypes

vmtksurfacetransform = 'vmtkSurfaceTransform'

class vmtkSurfaceTransform(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.MatrixCoefficients = []
        self.InvertMatrix = 0

        self.SetScriptName('vmtksurfacetransform')
        self.SetScriptDoc('transform a surface with a provided matrix')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'the input surface','vmtksurfacereader'],
            ['MatrixCoefficients','matrix','float',16,'coefficients of transform matrix'],
            ['InvertMatrix','invert','int',1,'invert matrix before applying transformation']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Surface == None):
            self.PrintError('Error: no Surface.')

        matrix = vtk.vtkMatrix4x4()
        matrix.DeepCopy(self.MatrixCoefficients)
        if self.InvertMatrix:
            matrix.Invert()

        transform = vtk.vtkMatrixToLinearTransform()
        transform.SetInput(matrix)

        transformFilter = vtk.vtkTransformPolyDataFilter()
        transformFilter.SetInput(self.Surface)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Surface = transformFilter.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
