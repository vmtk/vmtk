#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagereslice.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

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


class vmtkImageReslice(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.ReferenceImage = None

        self.OutputSpacing = []
        self.OutputOrigin = []
        self.OutputExtent = []

        self.Interpolation = 'linear'
        self.Cast = 1

        self.BackgroundLevel = 0.0

        self.MatrixCoefficients = []
        self.InvertMatrix = 0
        self.Matrix4x4 = None
        self.Rotation = [0.0,0.0,0.0]
        self.Translation = [0.0,0.0,0.0]
        self.Scaling = [1.0,1.0,1.0]

        self.TransformInputSampling = 1

        self.SetScriptName('vmtkimagereslice')
        self.SetScriptDoc('reslice an image based on user-specified parameters or on a reference image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ReferenceImage','r','vtkImageData',1,'','the reference image','vmtkimagereader'],
            ['OutputSpacing','spacing','float',3,'','the output spacing'],
            ['OutputOrigin','origin','float',3,'','the output origin'],
            ['OutputExtent','extent','int',6,'','the output extent'],
            ['Interpolation','interpolation','str',1,'["nearestneighbor","linear","cubic"]','interpolation during reslice'],
            ['Cast','cast','bool',1,'','toggle cast image to float type'],
            ['BackgroundLevel','background','float',1,'','the output image background'],
            ['Matrix4x4','matrix4x4','vtkMatrix4x4',1,'','the input transform matrix'],
            ['MatrixCoefficients','matrix','float',16,'','coefficients of transform matrix'],
            ['InvertMatrix','invert','bool',1,'','invert matrix before applying transformation'],
            ['Rotation','rotation','float',3,'','rotations around the x-,y- and z-axis'],
            ['Translation','translation','float',3,'','translation in the x-,y- and z-directions'],
            ['Scaling','scaling','float',3,'','scaling of the x-,y- and z-directions'],
            ['TransformInputSampling','transforminputsampling','bool',1,'','transform spacing, origin and extent of the Input (or the InformationInput) according to the direction cosines and origin of the ResliceAxes before applying them as the default output spacing, origin and extent']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Cast:
            cast = vtk.vtkImageCast()
            cast.SetInputData(self.Image)
            cast.SetOutputScalarTypeToFloat()
            cast.Update()
            self.Image = cast.GetOutput()

        resliceFilter = vtk.vtkImageReslice()
        resliceFilter.SetInputData(self.Image)
        if self.ReferenceImage:
            resliceFilter.SetInformationInput(self.ReferenceImage)
        else:
            if self.OutputSpacing:
                resliceFilter.SetOutputSpacing(self.OutputSpacing)
            if self.OutputOrigin:
                resliceFilter.SetOutputOrigin(self.OutputOrigin)
            if self.OutputExtent:
                resliceFilter.SetOutputExtent(self.OutputExtent)
        if self.Interpolation == 'nearestneighbor':
            resliceFilter.SetInterpolationModeToNearestNeighbor()
        elif self.Interpolation == 'linear':
            resliceFilter.SetInterpolationModeToLinear()
        elif self.Interpolation == 'cubic':
            resliceFilter.SetInterpolationModeToCubic()
        else:
            self.PrintError('Error: unsupported interpolation mode')
        resliceFilter.SetBackgroundLevel(self.BackgroundLevel)

        if self.TransformInputSampling:
            resliceFilter.TransformInputSamplingOn()
        else:
            resliceFilter.TransformInputSamplingOff()

        if not self.Matrix4x4:
            if self.MatrixCoefficients != []:
                self.PrintLog('Setting up transform matrix using specified coefficients')
                self.Matrix4x4 = vtk.vtkMatrix4x4()
                self.Matrix4x4.DeepCopy(self.MatrixCoefficients)
            elif self.Translation != [0.0,0.0,0.0] or self.Rotation != [0.0,0.0,0.0] or self.Scaling != [1.0,1.0,1.0]:
                self.PrintLog('Setting up transform matrix using specified translation, rotation and/or scaling')
                transform = vtk.vtkTransform()
                transform.RotateX(self.Rotation[0])
                transform.RotateY(self.Rotation[1])
                transform.RotateZ(self.Rotation[2])
                transform.Translate(self.Translation[0], self.Translation[1], self.Translation[2])
                transform.Scale(self.Scaling[0], self.Scaling[1], self.Scaling[2])
                self.Matrix4x4 = vtk.vtkMatrix4x4()
                self.Matrix4x4.DeepCopy(transform.GetMatrix())

        if self.InvertMatrix and self.Matrix4x4:
            self.Matrix4x4.Invert()

        if self.Matrix4x4:
            transform = vtk.vtkMatrixToLinearTransform()
            transform.SetInput(self.Matrix4x4)
            resliceFilter.SetResliceTransform(transform)

        resliceFilter.Update()

        self.Image = resliceFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
