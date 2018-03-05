#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagesmoothing.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes
from vmtk import vtkvmtk


class vmtkImageSmoothing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Image = None

        self.Method = 'gauss'
        self.StandardDeviation = 1.0
        self.RadiusFactor = 5.0
        self.Dimensionality = 3
        self.Conductance = 1.0
        self.NumberOfIterations = 5
        self.TimeStep = 0.0625
        self.EnhancedImage = None

        self.SetScriptName('vmtkimagesmoothing')
        self.SetScriptDoc('smooth an image with a Gaussian kernel or anisotropic diffusion')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Method','method','str',1,'["gauss","anisotropic"]'],
            ['StandardDeviation','sigma','float',1,'(0.0,)','the standard deviation of the Gaussian in real space units (gauss)'],
            ['RadiusFactor','radiusfactor','float',1,'(0.0,)','the factor specifing the width of the discrete Gaussian kernel (gauss)'],
            ['Dimensionality','dimensionality','int',1,'(2,3)','the dimensionality of the Aconvolution (gauss)'],
            ['Conductance','conductance','float',1,'(0.0,)','anisotropic diffustion coefficients (anisotropic)'],
            ['NumberOfIterations','iterations','int',1,'(5,)',' number of anisotropic diffusion iterationsl (anisotropic)'],
            ['TimeStep','timestep','float',1,'(0.0625,)','time step of anisotropic diffusion (anisotropic)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def ApplyGaussianFilter(self):

        spacing = self.Image.GetSpacing()
        pixelStandardDeviations = [self.StandardDeviation/spacing[0],self.StandardDeviation/spacing[1],self.StandardDeviation/spacing[2]]

        smoothingFilter = vtk.vtkImageGaussianSmooth()
        smoothingFilter.SetInputData(self.Image)
        smoothingFilter.SetStandardDeviations(pixelStandardDeviations)
        smoothingFilter.SetRadiusFactor(self.RadiusFactor)
        smoothingFilter.SetDimensionality(self.Dimensionality)
        smoothingFilter.Update()

        self.EnhancedImage = smoothingFilter.GetOutput()

    def ApplyAnisotropicDiffusion(self):
        grad = vtkvmtk.vtkvmtkAnisotropicDiffusionImageFilter()
        grad.SetInputData(self.Image)
        grad.SetNumberOfIterations(self.NumberOfIterations)
        grad.SetTimeStep(self.TimeStep)
        grad.SetConductance(self.Conductance)
        grad.Update()
        self.EnhancedImage = grad.GetOutput()

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Method == 'gauss':
            self.ApplyGaussianFilter()
        elif self.Method == 'anisotropic':
            self.ApplyAnisotropicDiffusion()
        else:
            self.PrintError('Error: unsupported vessel enhancement method')

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
