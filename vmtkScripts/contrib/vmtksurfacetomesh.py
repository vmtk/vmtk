#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetomesh.py,v $
## Language:  Python
## Date:      $Date: 2018/04/06 12:09:46 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.
##
## Contributed by Kurt Sansom

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes


class vmtkSurfaceToMesh(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Mesh = None
        self.CleanOutput = 0

        self.SetScriptName('vmtksurfacetomesh')
        self.SetScriptDoc('convert a surface to a mesh with no volume elements')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input mesh','vmtksurfacereader'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructredGrid',1,'','the output surface','vmtkmeshwriter']])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')
            
        if self.CleanOutput == 1:
            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputData(self.Surface)
            cleaner.Update()
            self.Surface = cleaner.GetOutput()

        surfaceToMeshFilter = vtk.vtkAppendFilter()
        surfaceToMeshFilter.AddInputData(self.Surface)
        surfaceToMeshFilter.Update()

        self.Mesh = surfaceToMeshFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
