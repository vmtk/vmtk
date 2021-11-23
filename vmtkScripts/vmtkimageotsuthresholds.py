#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageotsuthresholds.py,v $
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
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkImageOtsuThresholds(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.NumberOfHistogramBins = 128
        self.NumberOfThresholds = 1
        self.LabelOffset = 0
        self.OutputThresholdId = 0
        self.Thresholds = None
        self.Threshold = None

        self.SetScriptName('vmtkimageotsuthresholds')
        self.SetScriptDoc('automatically label an image using multiple Otsu thresholding')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['NumberOfHistogramBins','bins','int',1,'(0,)','the number of bins to use for generating the image histogram'],
            ['NumberOfThresholds','thresholds','int',1,'(0,)','the number of intensity thresholds to generate for generating the labeled image'],
            ['OutputThresholdId','othresholdid','int',1,'(0,)','the id of the threshold to send in output'],
            ['LabelOffset','offset','int',1,'(0,)','the lowest label value to use while generating the labeled image']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['Thresholds','othresholds','vtkFloatArray',1,'','the computed thresholds'],
            ['Threshold','othreshold','float',1,'','the nth computed threshold, with n set to OutputThresholdId']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        cast = vtk.vtkImageCast()
        cast.SetInputData(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()

        otsuFilter = vtkvmtk.vtkvmtkOtsuMultipleThresholdsImageFilter()
        otsuFilter.SetInputConnection(cast.GetOutputPort())
        otsuFilter.SetNumberOfHistogramBins(self.NumberOfHistogramBins)
        otsuFilter.SetNumberOfThresholds(self.NumberOfThresholds)
        otsuFilter.SetLabelOffset(self.LabelOffset)
        otsuFilter.Update()

        self.Image = otsuFilter.GetOutput()
        self.Thresholds = otsuFilter.GetThresholds()
        self.Threshold = self.Thresholds.GetValue(self.OutputThresholdId)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
