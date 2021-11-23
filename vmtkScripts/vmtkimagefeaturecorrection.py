#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagefeaturecorrection.py,v $
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
from vmtk import vtkvmtk


class vmtkImageFeatureCorrection(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.LevelSets = None
        self.Sigma = 1.0
        self.ScaleValue = 0.02
        self.NegateLevelSets = False
        self.ComputeScaleValueFromInput = True

        self.SetScriptName('vmtkimagefeaturecorrection')
        self.SetScriptDoc('correct a feature image (e.g. remove influence of bone and/or air from CT-based feature images)')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input feature image','vmtkimagereader'],
            ['LevelSets','levelsets','vtkImageData',1,'','the input level sets','vmtkimagereader'],
            ['Sigma','sigma','float',1,'(0.0,)'],
            ['ScaleValue','scale','float',1,'(0.0,)'],
            ['ComputeScaleValueFromInput','scalefrominput','bool',1,''],
            ['NegateLevelSets','negate','bool',1,'']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: No input image.')

        if self.LevelSets == None:
            self.PrintError('Error: No input level sets.')

        if self.NegateLevelSets:
            negateFilter = vtk.vtkImageMathematics()
            negateFilter.SetInputData(self.LevelSets)
            negateFilter.SetOperationToMultiplyByK()
            negateFilter.SetConstantK(-1.0)
            negateFilter.Update()
            self.LevelSets = negateFilter.GetOutput()

        sigmoid = vtkvmtk.vtkvmtkLevelSetSigmoidFilter()
        sigmoid.SetInputData(self.Image)
        sigmoid.SetLevelSetsImage(self.LevelSets)
        sigmoid.SetSigma(self.Sigma)
        sigmoid.SetScaleValue(self.ScaleValue)
        if self.ComputeScaleValueFromInput:
            sigmoid.ComputeScaleValueFromInputOn()
        else:
            sigmoid.ComputeScaleValueFromInputOff()
        sigmoid.Update()

        self.Image = sigmoid.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
