#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetransformtoras.py,v $
## Language:  Python
## Date:      $Date: Sun Feb 21 17:02:37 CET 2010$
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was improved by 
##       Hugo Gratama van Andel
##       Academic Medical Centre - University of Amsterdam
##       Dept. Biomedical Engineering  & Physics


import vtk
import sys

import pypes

vmtksurfacetransformtoras = 'vmtkSurfaceTransformToRAS'

class vmtkSurfaceTransformToRAS(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.RasToIjkMatrixCoefficients = None

        self.SetScriptName('vmtksurfacetransformtoras')
        self.SetScriptDoc('transform a surface generated in XYZ image space into RAS space')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['RasToIjkMatrixCoefficients','matrix','float',16,'','coefficients of RASToIJK transform matrix']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.RasToIjkMatrixCoefficients == None:
            self.PrintError('Error: no RasToIjkMatrixCoefficients.')

        matrix = vtk.vtkMatrix4x4()
        matrix.DeepCopy(self.RasToIjkMatrixCoefficients)
        matrix.Invert()

        origin = [matrix.GetElement(0,3), matrix.GetElement(1,3), matrix.GetElement(2,3)] 
        translationToOrigin = [-origin[0], -origin[1], -origin[2]] 

        for i in range(3):
            direction = [matrix.GetElement(0,i), matrix.GetElement(1,i), matrix.GetElement(2,i)]
            vtk.vtkMath.Normalize(direction)
            matrix.SetElement(0,i,direction[0])
            matrix.SetElement(1,i,direction[1])
            matrix.SetElement(2,i,direction[2])
        matrix.SetElement(0,3,0.0)
        matrix.SetElement(1,3,0.0)
        matrix.SetElement(2,3,0.0)
 
        transform = vtk.vtkTransform()
        transform.PostMultiply()
        transform.Translate(translationToOrigin)
        transform.Concatenate(matrix)
        transform.Translate(origin)

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
