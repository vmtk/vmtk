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

        self.SigmaMin = 0.2
        self.SigmaMax = 2.0
        self.NumberOfSigmaSteps = 10
        self.Alpha = 0.5
        self.Beta = 0.5
        self.Gamma = 5.0

        self.SetScriptName('vmtkimagevesselenhancement')
        self.SetScriptDoc('compute a feature image for use in segmentation')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Method','method','str',1,["frangi"]],
            ['SigmaMin','sigmamin','float',1,'(0.0,)'],
            ['SigmaMax','sigmamax','float',1,'(0.0,)'],
            ['NumberOfSigmaSteps','sigmasteps','int',1,'(0,)'],
            ['Alpha','alpha','float',1,'(0.0,)'],
            ['Beta','beta','float',1,'(0.0,)'],
            ['Gamma','gamma','float',1,'(0.0,)']
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

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Method == 'frangi':
          self.ApplyFrangiVesselness()
        else:
          self.PrintError('Error: unsupported vessel enhancement method')

        self.Image = self.EnhancedImage


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
