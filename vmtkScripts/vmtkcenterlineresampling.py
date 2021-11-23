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

from vmtk import vmtklineresampling


class vmtkCenterlineResampling(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.Length = 0.0;

        self.SetScriptName('vmtkcenterlineresampling')
        self.SetScriptDoc('resample input centerlines with a spline filter')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['Length','length','float',1,'(0.0,)','length of the resampling interval']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        lineResampling = vmtklineresampling.vmtkLineResampling()
        lineResampling.Surface = self.Centerlines
        lineResampling.Length = self.Length
        lineResampling.Execute()

        self.Centerlines = lineResampling.Surface


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
