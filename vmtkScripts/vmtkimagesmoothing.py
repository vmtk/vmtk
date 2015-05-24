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


import vtk
import sys

import pypes

vmtkimagesmoothing = 'vmtkImageSmoothing'

class vmtkImageSmoothing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Image = None

        self.StandardDeviation = 1.0
        self.RadiusFactor = 5.0
        self.Dimensionality = 3

        self.SetScriptName('vmtkimagesmoothing')
        self.SetScriptDoc('smooth an image with a Gaussian kernel')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['StandardDeviation','sigma','float',1,'(0.0,)','the standard deviation of the Gaussian in real space units'],
            ['RadiusFactor','radiusfactor','float',1,'(0.0,)','the factor specifing the width of the discrete Gaussian kernel'],
            ['Dimensionality','dimensionality','int',1,'(2,3)','the dimensionality of the convolution']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        spacing = self.Image.GetSpacing()
        pixelStandardDeviations = [self.StandardDeviation/spacing[0],self.StandardDeviation/spacing[1],self.StandardDeviation/spacing[2]]

        smoothingFilter = vtk.vtkImageGaussianSmooth()
        smoothingFilter.SetInputData(self.Image)
        smoothingFilter.SetStandardDeviations(pixelStandardDeviations)
        smoothingFilter.SetRadiusFactor(self.RadiusFactor)
        smoothingFilter.SetDimensionality(self.Dimensionality)
        smoothingFilter.Update()

        self.Image = smoothingFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
