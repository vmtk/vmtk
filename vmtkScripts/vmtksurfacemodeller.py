#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacemodeller.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceModeller(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Image = None
        self.SampleSpacing = -1.0
        self.NegativeInside = 1

        self.SetScriptName('vmtksurfacemodeller')
        self.SetScriptDoc('converts a surface to an image containing the signed distance transform from the surface points')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['SampleSpacing','samplespacing','float',1,'(0.0,)','spacing of the output image (isotropic)'],
            ['NegativeInside','negativeinside','bool',1,'','toggle sign of distance transform negative inside the surface']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        surfaceModellerFilter = vtk.vtkSurfaceReconstructionFilter()
        surfaceModellerFilter.SetInputData(self.Surface)
        surfaceModellerFilter.SetSampleSpacing(self.SampleSpacing)
        surfaceModellerFilter.SetNeighborhoodSize(40)
        surfaceModellerFilter.Update()
        self.Image = surfaceModellerFilter.GetOutput()

        if self.NegativeInside:
            negate = vtk.vtkImageMathematics()
            negate.SetInputData(self.Image)
            negate.SetConstantK(-1.0)
            negate.SetOperationToMultiplyByK()
            negate.Update()
            self.Image = negate.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
