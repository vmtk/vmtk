#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshlinearize.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import sys

import pypes

vmtkmeshlinearize = 'vmtkMeshLinearize'

class vmtkMeshLinearize(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Mesh = None

        self.SetScriptName('vmtkmeshlinearize')
        self.SetScriptDoc('convert the elements of a mesh to linear')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'the input mesh','vmtkmeshreader']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'the output mesh','vmtkmeshwriter']])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        linearToQuadraticFilter = vtk.vtkDataSetTriangleFilter()
        linearToQuadraticFilter.SetInput(self.Mesh)
        linearToQuadraticFilter.Update()

        self.Mesh = linearToQuadraticFilter.GetOutput()

        if self.Mesh.GetSource():
            self.Mesh.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
