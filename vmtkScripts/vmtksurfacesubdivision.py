#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacesubdivision.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceSubdivision(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.NumberOfSubdivisions = 1
        self.Method = 'linear'

        self.SetScriptName('vmtksurfacesubdivision')
        self.SetScriptDoc('subdivide a triangulated surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['NumberOfSubdivisions','subdivisions','int',1,'(0,)','number of triangle subdivisions'],
            ['Method','method','str',1,'["linear","butterfly","loop"]','subdivision method']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        subdivisionFilter = None
        if self.Method == 'linear':
            subdivisionFilter = vtk.vtkLinearSubdivisionFilter()
        elif self.Method == 'butterfly':
            subdivisionFilter = vtk.vtkButterflySubdivisionFilter()
        elif self.Method == 'loop':
            subdivisionFilter = vtk.vtkLoopSubdivisionFilter()
        else:
            self.PrintError('Error: Unsupported subdivision method.')
        subdivisionFilter.SetInputData(self.Surface)
        subdivisionFilter.SetNumberOfSubdivisions(self.NumberOfSubdivisions)
        subdivisionFilter.Update()

        self.Surface = subdivisionFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
