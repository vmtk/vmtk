#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vtkvmtkImageCurvedMPR.py,v $
## Language:  Python
## Date:      $Date: 2008/02/12 13.44 $
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

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkImageCurvedMPR(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Centerlines = None
        self.NormalsArrayName = 'ParallelTransportNormals'
        self.FrenetTangentArrayName = 'FrenetTangent'
        self.InplaneOutputSize = 100
        self.InplaneOutputSpacing = 1.0
        self.ReslicingBackgroundLevel = 0.0

        self.SetScriptName('vmtkimagecurvedmpr')
        self.SetScriptDoc('Make an MPR image from a centerline and an input image')

        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['NormalsArrayName','normalsarray','str',1,'','name of the array where parallel transport normals to the centerlines are stored'],
            ['InplaneOutputSize','size','int',1,'(1,)','size of the square in pixels that each resulting MPR image should have'],
            ['ReslicingBackgroundLevel','background','float',1,'','value of the pixels in the mpr image that are outside of the inputimage'],
            ['InplaneOutputSpacing','spacing','float',1,'(0.001,)','spacing between the pixels in the output MPR images'],
            ['FrenetTangentArrayName','frenettangentarray','str',1,'','name of the array where tangent vectors of the Frenet reference system are stored']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        curvedMPRImageFilter = vtkvmtk.vtkvmtkCurvedMPRImageFilter()
        curvedMPRImageFilter.SetInputData(self.Image)
        curvedMPRImageFilter.SetCenterline(self.Centerlines)
        curvedMPRImageFilter.SetParallelTransportNormalsArrayName(self.NormalsArrayName)
        curvedMPRImageFilter.SetFrenetTangentArrayName(self.FrenetTangentArrayName)
        curvedMPRImageFilter.SetInplaneOutputSpacing(self.InplaneOutputSpacing, self.InplaneOutputSpacing)
        curvedMPRImageFilter.SetInplaneOutputSize(self.InplaneOutputSize, self.InplaneOutputSize)
        curvedMPRImageFilter.SetReslicingBackgroundLevel(self.ReslicingBackgroundLevel)
        curvedMPRImageFilter.Update()

        self.Image = curvedMPRImageFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
