#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklineresampling.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkLineResampling(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.Length = 0.0;

        self.SetScriptName('vmtklineresampling')
        self.SetScriptDoc('resample input lines with a spline filter')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface containing vtkPolyLine cells','vmtksurfacereader'],
            ['Length','length','float',1,'(0.0,)','length of the resampling interval']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface containing the resampled vtkPolyLine cells','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Surface)
        cleaner.Update()

        if self.Length == 0.0:
            self.Length = cleaner.GetOutput().GetLength() / 100.0

        splineFilter = vtk.vtkSplineFilter()
        splineFilter.SetInputConnection(cleaner.GetOutputPort())
        splineFilter.SetSubdivideToLength()
        splineFilter.SetLength(self.Length)
        splineFilter.Update()

        self.Surface = splineFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
