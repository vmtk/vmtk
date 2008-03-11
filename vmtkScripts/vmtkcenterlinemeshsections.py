#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinemeshsections.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtkcenterlinemeshsections = 'vmtkCenterlineMeshSections'

class vmtkCenterlineMeshSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
       
        self.Mesh = None
        self.Centerlines = None
        self.CenterlineSections = None

        self.SetScriptName('vmtkcenterlinemeshsections')
        self.SetScriptDoc('extract mesh sections along centerlines. The script takes in input the mesh and the relative centerlines.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader']
            ])
        self.SetOutputMembers([
            ['CenterlineSections','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineSections = vtkvmtk.vtkvmtkUnstructuredGridCenterlineSections()
        centerlineSections.SetInput(self.Mesh)
        centerlineSections.SetCenterlines(self.Centerlines)
        centerlineSections.Update()

        self.CenterlineSections = centerlineSections.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
