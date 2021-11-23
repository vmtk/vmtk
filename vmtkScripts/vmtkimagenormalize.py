#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagenormalize.py,v $
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


class vmtkImageNormalize(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.SetScriptName('vmtkimagenormalize')
        self.SetScriptDoc('automatically label an image using multiple Otsu thresholding')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        cast = vtk.vtkImageCast()
        cast.SetInputData(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()

        normalizeFilter = vtkvmtk.vtkvmtkNormalizeImageFilter()
        normalizeFilter.SetInputConnection(cast.GetOutputPort())
        normalizeFilter.Update()

        self.Image = normalizeFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
