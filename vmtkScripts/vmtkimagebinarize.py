#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagebinarize.py,v $
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


class vmtkImageBinarize(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.Threshold = 0.0
        self.LowerLabel = 0
        self.UpperLabel = 1

        self.SetScriptName('vmtkimagebinarize')
        self.SetScriptDoc('binarize the input image producing a label map')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Threshold','threshold','float',1,'','the threshold at which the image should be binarized'],
            ['LowerLabel','lowerlabel','int',1,'','the label to associate to the pixels lower than the threshold'],
            ['UpperLabel','upperlabel','int',1,'','the label to associate to the pixels higher than the threshold']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        thresholdFilter = vtk.vtkImageThreshold()
        thresholdFilter.SetInputData(self.Image)
        thresholdFilter.ThresholdByUpper(self.Threshold)
        thresholdFilter.ReplaceInOn()
        thresholdFilter.ReplaceOutOn()
        thresholdFilter.SetInValue(self.UpperLabel)
        thresholdFilter.SetOutValue(self.LowerLabel)
        thresholdFilter.SetOutputScalarTypeToShort()
        thresholdFilter.Update()

        self.Image = thresholdFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
