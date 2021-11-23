#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageshiftscale.py,v $
## Language:  Python
## Date:      $Date: 2009/01/09 13:56:00 $
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Hugo Gratama van Andel
##       Academic Medical Centre - University of Amsterdam
##       Dept. Biomedical Engineering  & Physics

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkImageShiftScale(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.OutputType = 'unchanged'
        self.Shift = 0.0
        self.Scale = 1.0

        self.MapRanges = 0
        self.InputRange = [0.0,0.0]
        self.OutputRange = [0.0,0.0]

        self.ClampOverflowOn = 1

        self.SetScriptName('vmtkimageshiftscale')
        self.SetScriptDoc('shift and scale the intensity of an image and cast it to a specified type')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['OutputType','type','str',1,'["unchanged","float","double","uchar","char","ushort","short","long","ulong","int","uint"]','the output image type - use "unchanged", the default, to keep same type as input'],
            ['ClampOverflowOn','clamp','bool',1,'','Whith ClampOverflow On, the data is thresholded so that the output value does not exceed the max or min of the data type'],
            ['Shift','shift','float',1,'','the shift value'],
            ['Scale','scale','float',1,'','the scale value'],
            ['MapRanges','mapranges','bool',1,'','toggle mapping of input range to output range instead of simple shift scale'],
            ['InputRange','inputrange','float',2,'','the input range that will be mapped in the output range - leave default or set to 0.0 0.0 for using input image scalar range as input range'],
            ['OutputRange','outputrange','float',2,'','the output range into which the input range will be mapped']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.MapRanges:
            if self.InputRange == [0.0,0.0]:
                self.InputRange = self.Image.GetScalarRange()
            if self.InputRange[1] == self.InputRange[0]:
                self.PrintError('Error: Input range has zero width. Cannot map values')
            self.Shift = -self.InputRange[0]
            self.Scale = (self.OutputRange[1] - self.OutputRange[0]) / (self.InputRange[1] - self.InputRange[0])

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInputData(self.Image)
        shiftScale.SetShift(self.Shift)
        shiftScale.SetScale(self.Scale)

        if self.OutputType == 'double':
            shiftScale.SetOutputScalarTypeToDouble()
        elif self.OutputType == 'uchar':
            shiftScale.SetOutputScalarTypeToUnsignedChar()
        elif self.OutputType == 'char':
            shiftScale.SetOutputScalarTypeToChar()
        elif self.OutputType == 'ushort':
            shiftScale.SetOutputScalarTypeToUnsignedShort()
        elif self.OutputType == 'short':
            shiftScale.SetOutputScalarTypeToShort()
        elif self.OutputType == 'ulong':
            shiftScale.SetOutputScalarTypeToUnsignedLong()
        elif self.OutputType == 'long':
            shiftScale.SetOutputScalarTypeToLong()
        elif self.OutputType == 'uint':
            shiftScale.SetOutputScalarTypeToUnsignedInt()
        elif self.OutputType == 'int':
            shiftScale.SetOutputScalarTypeToInt()
        elif self.OutputType == 'float':
            shiftScale.SetOutputScalarTypeToFloat()

        if self.OutputType != 'unchanged':
            if self.ClampOverflowOn:
                shiftScale.ClampOverflowOn()

        shiftScale.ClampOverflowOff()
        shiftScale.Update()

        self.Image = shiftScale.GetOutput()

        if self.MapRanges and self.OutputRange[0] != 0.0:
            shiftScale2 = vtk.vtkImageShiftScale()
            shiftScale2.SetInputData(self.Image)
            shiftScale2.SetShift(self.OutputRange[0])
            shiftScale2.SetScale(1.0)
            shiftScale2.Update()
            self.Image = shiftScale2.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
