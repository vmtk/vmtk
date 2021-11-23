#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinesmoothing.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:52:56 $
## Version:   $Revision: 1.1 $

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


class vmtkCenterlineSmoothing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.NumberOfSmoothingIterations = 100
        self.SmoothingFactor = 0.1

        self.SetScriptName('vmtkcenterlinesmoothing')
        self.SetScriptDoc('smooth centerlines with a moving average filter')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['NumberOfSmoothingIterations','iterations','int',1,'(0,)'],
            ['SmoothingFactor','factor','float',1,'(0.0,)']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineSmoothing = vtkvmtk.vtkvmtkCenterlineSmoothing()
        centerlineSmoothing.SetInputData(self.Centerlines)
        centerlineSmoothing.SetNumberOfSmoothingIterations(self.NumberOfSmoothingIterations)
        centerlineSmoothing.SetSmoothingFactor(self.SmoothingFactor)
        centerlineSmoothing.Update()

        self.Centerlines = centerlineSmoothing.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
