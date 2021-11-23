#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtransform.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was improved by
##       Hugo Gratama van Andel
##       Academic Medical Centre - University of Amsterdam
##       Dept. Biomedical Engineering  & Physics

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkMeshTransform(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.MatrixCoefficients = []
        self.InvertMatrix = 0
        self.Matrix4x4 = None
        self.Rotation = [0.0,0.0,0.0]
        self.Translation = [0.0,0.0,0.0]
        self.Scaling = [1.0,1.0,1.0]

        #TODO: define covariant vector array names

        self.SetScriptName('vmtkmeshtransform')
        self.SetScriptDoc('transform a mesh with a provided matrix')
        self.SetInputMembers([
            ['Mesh','i','vtkPolyData',1,'','the input mesh','vmtkmeshreader'],
            ['Matrix4x4','matrix4x4','vtkMatrix4x4',1,'','the input transform matrix'],
            ['MatrixCoefficients','matrix','float',16,'','coefficients of transform matrix'],
            ['InvertMatrix','invert','bool',1,'','invert matrix before applying transformation'],
            ['Rotation','rotation','float',3,'','rotations around the x-,y- and z-axis'],
            ['Translation','translation','float',3,'','translation in the x-,y- and z-directions'],
            ['Scaling','scaling','float',3,'','scaling of the x-,y- and z-directions']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkPolyData',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        if not self.Matrix4x4:
            self.Matrix4x4 = vtk.vtkMatrix4x4()
            if self.MatrixCoefficients != []:
                self.PrintLog('Setting up transform matrix using specified coefficients')
                self.Matrix4x4.DeepCopy(self.MatrixCoefficients)
            elif self.Translation != [0.0,0.0,0.0] or self.Rotation != [0.0,0.0,0.0] or self.Scaling != [1.0,1.0,1.0]:
                self.PrintLog('Setting up transform matrix using specified translation, rotation and/or scaling')
                transform = vtk.vtkTransform()
                transform.RotateX(self.Rotation[0])
                transform.RotateY(self.Rotation[1])
                transform.RotateZ(self.Rotation[2])
                transform.Translate(self.Translation[0], self.Translation[1], self.Translation[2])
                transform.Scale(self.Scaling[0], self.Scaling[1], self.Scaling[2])
                self.Matrix4x4.DeepCopy(transform.GetMatrix())

        if self.InvertMatrix:
            self.Matrix4x4.Invert()

        transform = vtk.vtkMatrixToLinearTransform()
        transform.SetInput(self.Matrix4x4)

        transformFilter = vtk.vtkTransformFilter()
        transformFilter.SetInputData(self.Mesh)
        transformFilter.SetTransform(transform)
        transformFilter.Update()

        self.Mesh = transformFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
