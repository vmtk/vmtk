#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevesselenhancement.py,v $
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


class vmtkImageVesselEnhancement(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Method = "frangi"

        self.EnhancedImage = None

        self.SigmaMin = 1.0
        self.SigmaMax = 1.0
        self.NumberOfSigmaSteps = 1
        self.SigmaStepMethod = 'equispaced'

        self.ScaledVesselness = 0

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
        self.BrightObject = True

        self.SetScriptName('vmtkimagevesselenhancement')
        self.SetScriptDoc('compute a feature image for use in segmentation')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Method','method','str',1,'["frangi","sato","ved","vedm"]'],
            ['SigmaMin','sigmamin','float',1,'(0.0,)'],
            ['SigmaMax','sigmamax','float',1,'(0.0,)'],
            ['NumberOfSigmaSteps','sigmasteps','int',1,'(0,)'],
            ['SigmaStepMethod','stepmethod','str',1,'["equispaced","logarithmic"]'],
            ['ScaledVesselness','scaled','bool',1,'','(frangi)'],
            ['BrightObject','brightobject','bool',1,'','(frangi)'],
            ['Alpha1','alpha1','float',1,'(0.0,)','(sato)'],
            ['Alpha2','alpha2','float',1,'(0.0,)','(sato)'],
            ['Alpha','alpha','float',1,'(0.0,)','(frangi, ved, vedm)'],
            ['Beta','beta','float',1,'(0.0,)','(frangi, ved, vedm)'],
            ['Gamma','gamma','float',1,'(0.0,)','(frangi, ved, vedm)'],
            ['C','c','float',1,'(0.0,)','(ved)'],
            ['TimeStep','timestep','float',1,'(0.0,)','(ved, vedm)'],
            ['Epsilon','epsilon','float',1,'(0.0,)','(ved, vedm)'],
            ['WStrength','wstrength','float',1,'(0.0,)','(ved, vedm)'],
            ['Sensitivity','sensitivity','float',1,'(0.0,)','(ved, vedm)'],
            ['NumberOfIterations','iterations','int',1,'(0,)','(ved, vedm)'],
            ['NumberOfDiffusionSubIterations','subiterations','int',1,'(1,)','(ved, vedm)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def ApplyFrangiVesselness(self):

        vesselness = vtkvmtk.vtkvmtkVesselnessMeasureImageFilter()
        vesselness.SetInputData(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetUseScaledVesselness(self.ScaledVesselness)
        vesselness.SetAlpha(self.Alpha)
        vesselness.SetBeta(self.Beta)
        vesselness.SetGamma(self.Gamma)
        vesselness.SetBrightObject(self.BrightObject)
        if self.SigmaStepMethod == 'equispaced':
            vesselness.SetSigmaStepMethodToEquispaced()
        elif self.SigmaStepMethod == 'logarithmic':
            vesselness.SetSigmaStepMethodToLogarithmic()
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def ApplySatoVesselness(self):

        vesselness = vtkvmtk.vtkvmtkSatoVesselnessMeasureImageFilter()
        vesselness.SetInputData(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetAlpha1(self.Alpha1)
        vesselness.SetAlpha2(self.Alpha2)
        if self.SigmaStepMethod == 'equispaced':
            vesselness.SetSigmaStepMethodToEquispaced()
        elif self.SigmaStepMethod == 'logarithmic':
            vesselness.SetSigmaStepMethodToLogarithmic()
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def ApplyVED(self):

        vesselness = vtkvmtk.vtkvmtkVesselEnhancingDiffusionImageFilter()
        vesselness.SetInputData(self.Image)
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
        if self.SigmaStepMethod == 'equispaced':
            vesselness.SetSigmaStepMethodToEquispaced()
        elif self.SigmaStepMethod == 'logarithmic':
            vesselness.SetSigmaStepMethodToLogarithmic()
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def ApplyVEDManniesing(self):

        vesselness = vtkvmtk.vtkvmtkVesselEnhancingDiffusion3DImageFilter()
        vesselness.SetInputData(self.Image)
        vesselness.SetSigmaMin(self.SigmaMin)
        vesselness.SetSigmaMax(self.SigmaMax)
        vesselness.SetNumberOfSigmaSteps(self.NumberOfSigmaSteps)
        vesselness.SetAlpha(self.Alpha)
        vesselness.SetBeta(self.Beta)
        vesselness.SetGamma(self.Gamma)
        vesselness.SetTimeStep(self.TimeStep)
        vesselness.SetEpsilon(self.Epsilon)
        vesselness.SetOmega(self.WStrength)
        vesselness.SetSensitivity(self.Sensitivity)
        vesselness.SetNumberOfIterations(self.NumberOfIterations)
        vesselness.SetRecalculateVesselness(self.NumberOfDiffusionSubIterations)
        if self.SigmaStepMethod == 'equispaced':
            vesselness.SetSigmaStepMethodToEquispaced()
        elif self.SigmaStepMethod == 'logarithmic':
            vesselness.SetSigmaStepMethodToLogarithmic()
        vesselness.Update()

        self.EnhancedImage = vtk.vtkImageData()
        self.EnhancedImage.DeepCopy(vesselness.GetOutput())

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.SigmaMax < self.SigmaMin:
            self.SigmaMax = self.SigmaMin

        if( self.Image.GetScalarType() != vtk.VTK_FLOAT):
          # filters only work on float
          print("input type not of type float, casting to float")
          #TODO use rescale filter for proper mapping
          cast = vtk.vtkImageCast()
          cast.SetInputData(self.Image)
          cast.SetOutputScalarTypeToFloat()
          cast.Update()
          self.Image.DeepCopy(cast.GetOutput())

        if(self.Image.GetDataDimension() != 3):
          self.PrintError('Error: unsupported image dimension, expected {0}D image'.format(3))

        if self.Method == 'frangi':
            self.ApplyFrangiVesselness()
        elif self.Method == 'sato':
            self.ApplySatoVesselness()
        elif self.Method == 'ved':
            self.ApplyVED()
        elif self.Method == 'vedm':
            self.ApplyVEDManniesing()
        else:
            self.PrintError('Error: unsupported vessel enhancement method')

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
