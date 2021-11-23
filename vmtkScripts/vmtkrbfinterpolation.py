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
from vmtk import vtkvmtk
import vtk
import sys

from vmtk import pypes


class vmtkRBFInterpolation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Seeds = None
        self.RBFType = 'biharmonic'

        self.Image = None

        self.Dimensions = [0, 0, 0]
        self.Bounds = [0.0, 1.0, 0.0, 1.0, 0.0, 1.0]

        self.SetScriptName('vmtkrbfinterpolation')
        self.SetScriptDoc('perform RBF interpolation from a set of seeds')
        self.SetInputMembers([
            ['Seeds','i','vtkPolyData',1,'','the input seeds','vmtksurfacereader'],
            ['Image','r','vtkImageData',1,'','the reference image','vmtkimagereader'],
            ['Dimensions','dimensions','int',3,''],
            ['Bounds','bounds','float',6,''],
            ['RBFType','rbftype','str',1,'["thinplatespline","biharmonic","triharmonic"]','the type of RBF interpolation']

            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Seeds == None:
            self.PrintError('Error: No input seeds.')

        rbf = vtkvmtk.vtkvmtkRBFInterpolation()
        rbf.SetSource(self.Seeds)
        if self.RBFType == "thinplatespline":
            rbf.SetRBFTypeToThinPlateSpline()
        elif self.RBFType == "biharmonic":
            rbf.SetRBFTypeToBiharmonic()
        elif self.RBFType == "triharmonic":
            rbf.SetRBFTypeToTriharmonic()
        rbf.ComputeCoefficients()

        if self.Image:
            origin = self.Image.GetOrigin()
            spacing = self.Image.GetSpacing()
            extent = self.Image.GetExtent()
            dimensions = self.Image.GetDimensions()
            modelBounds = [0.0,0.0,0.0,0.0,0.0,0.0]
            modelBounds[0] = origin[0] + spacing[0]*extent[0]
            modelBounds[1] = origin[0] + spacing[0]*extent[1]
            modelBounds[2] = origin[1] + spacing[1]*extent[2]
            modelBounds[3] = origin[1] + spacing[1]*extent[3]
            modelBounds[4] = origin[2] + spacing[2]*extent[4]
            modelBounds[5] = origin[2] + spacing[2]*extent[5]
        else:
            dimensions = self.Dimensions
            modelBounds = self.Bounds

        sampleFunction = vtk.vtkSampleFunction()
        sampleFunction.SetImplicitFunction(rbf)
        sampleFunction.SetOutputScalarTypeToDouble()
        sampleFunction.SetSampleDimensions(dimensions)
        sampleFunction.SetModelBounds(modelBounds)
        sampleFunction.CappingOff()
        sampleFunction.ComputeNormalsOff()
        sampleFunction.Update()

        self.Image = sampleFunction.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
