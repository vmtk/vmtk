#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagesmoothing.py,v $
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


class vmtkImageSmoothing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.Method = 'gauss'
        self.StandardDeviation = 1.0
        self.RadiusFactor = 5.0
        self.Dimensionality = 3
        self.EnhancedImage = None

        # This is the default value from Slicer 3 which assums pixel spacing = 1.0: (PixelSpacing)/2^{N+1}->1.0/2^{3+1}
        # If AutoCalculateTimeStep is enabled, this will be overwritten with the value calculated with the actual image
        # pixel spacing and number of dimensions.
        self.TimeStep = 0.0625
        self.AutoCalculateTimeStep = 1
        self.Conductance = 1.0
        self.NumberOfIterations = 5

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
            ['TimeStep','timestep','float',1,'(1.0E-16,1.0e16)','time step of anisotropic diffusion (anisotropic)'],
            ['AutoCalculateTimeStep','autocalculatetimestep','bool',1,'','auto calculate minimum time step (anisotropic)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['TimeStep','timestep','float',1],
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

        if self.AutoCalculateTimeStep:
            # equation to calculate the timestep is (PixelSpacing)/2^{N+1}
            minspacing = min(self.Image.GetSpacing())
            #multiply by .995 to make sure its under stability threshold
            self.TimeStep = (minspacing / (2.0 ** (self.Image.GetDataDimension() + 1))) * 0.9995

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

        # need to ensure that self.Method variable is a "string"
        # in order to ensure unicode compatibility in python 2 and 3, we check what python version is running,
        # and assign 'str' type if Py3, and 'basestring' type if Py2
        try:
            basestring
        except NameError:
            basestring = str

        if not isinstance(self.Method, basestring):
            self.PrintError('Method Selected (', self.Method, ') is not string type')

        if self.Method.lower() == 'gauss':
            self.ApplyGaussianFilter()
        elif self.Method.lower() == 'anisotropic':
            self.ApplyAnisotropicDiffusion()
        else:
            self.PrintError('Error: unsupported vessel enhancement method (', self.Method, ').')

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
