#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageobjectenhancement.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes
from vmtk import vtkvmtk


class vmtkImageObjectEnhancement(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.ScalesImage = None
        self.EnhancedImage = None

        self.SigmaMin = 1.0
        self.SigmaMax = 1.0
        self.NumberOfSigmaSteps = 1

        self.ScaledObjectness = 0

        self.Alpha = 0.5
        self.Beta = 0.5
        self.Gamma = 5.0

        self.ObjectDimension = 0

        self.SetScriptName('vmtkimageobjectenhancement')
        self.SetScriptDoc('compute a feature image for use in segmentation')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['SigmaMin','sigmamin','float',1,'(0.0,)'],
            ['SigmaMax','sigmamax','float',1,'(0.0,)'],
            ['NumberOfSigmaSteps','sigmasteps','int',1,'(0,)'],
            ['ScaledObjectness','scaled','bool',1,''],
            ['Alpha','alpha','float',1,'(0.0,)',''],
            ['Beta','beta','float',1,'(0.0,)',''],
            ['Gamma','gamma','float',1,'(0.0,)',''],
            ['ObjectDimension','dimension','int',1,'(0,2)','']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['ScalesImage','oscales','vtkImageData',1,'','the scales image','vmtkimagewriter']
            ])

    def ApplyObjectness(self):

        objectness = vtkvmtk.vtkvmtkObjectnessMeasureImageFilter()
        objectness.SetInputData(self.Image)
        objectness.SetSigmaMin(self.SigmaMin)
        objectness.SetSigmaMax(self.SigmaMax)
        objectness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        objectness.SetAlpha(self.Alpha)
        objectness.SetBeta(self.Beta)
        objectness.SetGamma(self.Gamma)
        objectness.SetObjectDimension(self.ObjectDimension)
        objectness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(objectness.GetOutput())

        self.ScalesImage = vtk.vtkImageData()
        self.ScalesImage.DeepCopy(objectness.GetScalesOutput())

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.SigmaMax < self.SigmaMin:
            self.SigmaMax = self.SigmaMin

        self.ApplyObjectness()

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
