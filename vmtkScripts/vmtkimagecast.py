#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagereslice.py,v $
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


class vmtkImageCast(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.OutputType = 'float'
        self.ShiftScale = 1
        self.WindowLevel = [0.0,0.0]

        self.SetScriptName('vmtkimagecast')
        self.SetScriptDoc('cast an image to a specified type')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['OutputType','type','str',1,'["float","double","uchar","short"]','the output image type'],
            ['ShiftScale','shiftscale','int',1,'','shift scale values to fit windowlevel'],
            ['WindowLevel','windowlevel','float',2,'','window and level for value mapping']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.OutputType == 'uchar' and self.ShiftScale:
            shiftScale = vtk.vtkImageShiftScale()
            shiftScale.SetInputData(self.Image)
            if self.WindowLevel[0] == 0.0:
                scalarRange = self.Image.GetScalarRange()
                scale = 255.0
                if (scalarRange[1]-scalarRange[0]) > 0.0:
                    scale = 255.0/(scalarRange[1]-scalarRange[0])
                shiftScale.SetShift(-scalarRange[0])
                shiftScale.SetScale(scale)
            else:
                shiftScale.SetShift(-(self.WindowLevel[1]-self.WindowLevel[0]/2.0))
                shiftScale.SetScale(255.0/self.WindowLevel[0])
            shiftScale.SetOutputScalarTypeToUnsignedChar()
            shiftScale.ClampOverflowOn()
            shiftScale.Update()
            self.Image = shiftScale.GetOutput()
        else:
            cast = vtk.vtkImageCast()
            cast.SetInputData(self.Image)
            if self.OutputType == 'float':
                cast.SetOutputScalarTypeToFloat()
            elif self.OutputType == 'double':
                cast.SetOutputScalarTypeToDouble()
            elif self.OutputType == 'uchar':
                cast.SetOutputScalarTypeToUnsignedChar()
            elif self.OutputType == 'short':
                cast.SetOutputScalarTypeToShort()
            cast.Update()
            self.Image = cast.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
