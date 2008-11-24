#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevesselenhancement.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys

import pypes
import vtkvmtk

vmtkimagevesselenhancement = 'vmtkImageVesselEnhancement'

class vmtkImageVesselEnhancement(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Image = None
        self.Method = "frangi"
  
        self.EnhancedImage = None

        self.SigmaMin = 1.0
        self.SigmaMax = 1.0
        self.NumberOfSigmaSteps = 1

        self.Alpha = 0.5
        self.Beta = 0.5
        self.Gamma = 5.0
        self.Alpha1 = 0.5
        self.Alpha2 = 2.0
        self.C = 1E-6
        self.TimeStep = 1E-2
        self.Epsilon = 1E-2
        self.WStrength = 25.0
        self.Sensitivity = 5.0
        self.NumberOfIterations = 0
        self.NumberOfDiffusionSubIterations = 0

        self.SetScriptName('vmtkimagevesselenhancement')
        self.SetScriptDoc('compute a feature image for use in segmentation')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Method','method','str',1,'["frangi","sato","ved"]'],
            ['SigmaMin','sigmamin','float',1,'(0.0,)'],
            ['SigmaMax','sigmamax','float',1,'(0.0,)'],
            ['NumberOfSigmaSteps','sigmasteps','int',1,'(0,)'],
            ['Alpha1','alpha1','float',1,'(0.0,)','(sato)'],
            ['Alpha2','alpha2','float',1,'(0.0,)','(sato)'],
            ['Alpha','alpha','float',1,'(0.0,)','(frangi, ved)'],
            ['Beta','beta','float',1,'(0.0,)','(frangi, ved)'],
            ['Gamma','gamma','float',1,'(0.0,)','(frangi, ved)'],
            ['C','c','float',1,'(0.0,)','(ved)'],
            ['TimeStep','timestep','float',1,'(0.0,)','(ved)'],
            ['Epsilon','epsilon','float',1,'(0.0,)','(ved)'],
            ['WStrength','wstrength','float',1,'(0.0,)','(ved)'],
            ['Sensitivity','sensitivity','float',1,'(0.0,)','(ved)'],
            ['NumberOfIterations','iterations','int',1,'(0,)','(ved)'],
            ['NumberOfDiffusionSubIterations','subiterations','int',1,'(1,)','(ved)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def ApplyFrangiVesselness(self):

        vesselness = vtkvmtk.vtkvmtkVesselnessMeasureImageFilter()
        vesselness.SetInput(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetAlpha(self.Alpha)
        vesselness.SetBeta(self.Beta)
        vesselness.SetGamma(self.Gamma)
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def ApplySatoVesselness(self):

        vesselness = vtkvmtk.vtkvmtkSatoVesselnessMeasureImageFilter()
        vesselness.SetInput(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetAlpha1(self.Alpha1)
        vesselness.SetAlpha2(self.Alpha2)
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def ApplyVED(self):

        vesselness = vtkvmtk.vtkvmtkVesselEnhancingDiffusionImageFilter()
        vesselness.SetInput(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetAlpha(self.Alpha)
        vesselness.SetBeta(self.Beta)
        vesselness.SetGamma(self.Gamma)
        vesselness.SetC(self.C)
        vesselness.SetTimeStep(self.TimeStep)
        vesselness.SetEpsilon(self.Epsilon)
        vesselness.SetWStrength(self.WStrength)
        vesselness.SetSensitivity(self.Sensitivity)
        vesselness.SetNumberOfIterations(self.NumberOfIterations)
        vesselness.SetNumberOfDiffusionSubIterations(self.NumberOfDiffusionSubIterations)
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.SigmaMax < self.SigmaMin:
            self.SigmaMax = self.SigmaMin

        if self.Method == 'frangi':
            self.ApplyFrangiVesselness()
        elif self.Method == 'sato':
            self.ApplySatoVesselness()
        elif self.Method == 'ved':
            self.ApplyVED()
        else:
            self.PrintError('Error: unsupported vessel enhancement method')

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
