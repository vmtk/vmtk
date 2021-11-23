#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtosurface.py,v $
## Language:  Python
## Date:      $Date: 2006/04/06 16:47:46 $
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


class vmtkMeshToSurface(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Mesh = None
        self.CleanOutput = 0

        self.SetScriptName('vmtkmeshtosurface')
        self.SetScriptDoc('convert a mesh to a surface by throwing out volume elements and (optionally) the relative points')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        meshToSurfaceFilter = vtk.vtkGeometryFilter()
        meshToSurfaceFilter.SetInputData(self.Mesh)
        meshToSurfaceFilter.Update()

        self.Surface = meshToSurfaceFilter.GetOutput()

        if self.CleanOutput == 1:
            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputConnection(meshToSurfaceFilter.GetOutputPort())
            cleaner.Update()
            self.Surface = cleaner.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
