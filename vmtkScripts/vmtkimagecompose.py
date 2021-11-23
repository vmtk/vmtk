#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagecompose.py,v $
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


class vmtkImageCompose(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Image2 = None
        self.Operation = 'min'
        self.NegateImage2 = False

        self.SetScriptName('vmtkimagecompose')
        self.SetScriptDoc('compose an image based on user-specified parameters or on a reference image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Image2','i2','vtkImageData',1,'','the second input image','vmtkimagereader'],
            ['Operation','operation','str',1,'["min","max","multiply","subtract"]','the operation used to compose images'],
            ['NegateImage2','negatei2','bool',1,'','negate the second input before composing']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Image2 == None:
            self.PrintError('Error: No input image2.')

        if self.NegateImage2:
            negateFilter = vtk.vtkImageMathematics()
            negateFilter.SetInputData(self.Image2)
            negateFilter.SetOperationToMultiplyByK()
            negateFilter.SetConstantK(-1.0)
            negateFilter.Update()
            self.Image2 = negateFilter.GetOutput()

        composeFilter = vtk.vtkImageMathematics()
        composeFilter.SetInput1Data(self.Image)
        composeFilter.SetInput2Data(self.Image2)
        if self.Operation == 'min':
            composeFilter.SetOperationToMin()
        elif self.Operation == 'max':
            composeFilter.SetOperationToMax()
        elif self.Operation == 'multiply':
            composeFilter.SetOperationToMultiply()
        elif self.Operation == 'subtract':
            composeFilter.SetOperationToSubtract()
        else:
            self.PrintError('Error: Unsupported operation')
        composeFilter.Update()

        self.Image = composeFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
