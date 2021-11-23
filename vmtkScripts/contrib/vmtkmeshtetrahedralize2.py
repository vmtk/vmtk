#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtetrahedralize2.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## Bug fix for VTK 5.0

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes

from vmtk import vtkvmtk


class vmtkMeshTetrahedralize2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.TetrahedraOnly = 0

        self.SetScriptName('vmtkmeshtetrahedralize')
        self.SetScriptDoc('convert the elements of a mesh to linear')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['TetrahedraOnly','tetonly','bool',1,'','toggle suppression of 1D and 2D cells']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        tetrahedralizeFilter = vtk.vtkDataSetTriangleFilter()
        tetrahedralizeFilter.SetInputData(self.Mesh)
        if ((vtk.vtkVersion.GetVTKMajorVersion()>=5) and (vtk.vtkVersion.GetVTKMinorVersion()>0)):
            tetrahedralizeFilter.SetTetrahedraOnly(self.TetrahedraOnly)
        tetrahedralizeFilter.Update()

        self.Mesh = tetrahedralizeFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
