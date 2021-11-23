#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacenormals.py,v $
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
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkSurfaceNormals(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.NormalsArrayName = 'Normals'

        self.ComputeCellNormals = 0
        self.AutoOrientNormals = 1
        self.FlipNormals = 0
        self.Consistency = 1

        self.SetScriptName('vmtksurfacenormals')
        self.SetScriptDoc('compute normals to a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['NormalsArrayName','normalsarray','str',1,'','name of the array where normals have to be stored'],
            ['AutoOrientNormals','autoorientnormals','bool',1,'','try to auto orient normals outwards'],
            ['Consistency','consistency','bool',1,'','try to orient normals so that neighboring points have similar orientations'],
            ['ComputeCellNormals','cellnormals','bool',1,'','compute cell normals instead of point normals'],
            ['FlipNormals','flipnormals','bool',1,'','flip normals after computing them']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No Surface.')

        normalsFilter = vtk.vtkPolyDataNormals()
        normalsFilter.SetInputData(self.Surface)
        normalsFilter.SetAutoOrientNormals(self.AutoOrientNormals)
        normalsFilter.SetFlipNormals(self.FlipNormals)
        normalsFilter.SetConsistency(self.Consistency)
        normalsFilter.SetComputeCellNormals(self.ComputeCellNormals)
        normalsFilter.SplittingOff()
        normalsFilter.Update()

        self.Surface = normalsFilter.GetOutput()

        if self.NormalsArrayName != '':
            self.Surface.GetPointData().GetNormals().SetName(self.NormalsArrayName)


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
