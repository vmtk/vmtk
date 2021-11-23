#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlineattributes.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
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


class vmtkCenterlineAttributes(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.AbscissasArrayName = 'Abscissas'
        self.NormalsArrayName = 'ParallelTransportNormals'

        self.SetScriptName('vmtkcenterlineattributes')
        self.SetScriptDoc('compute centerline attributes like abscissa and parallel transport normals; this is a requried step for mapping')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['AbscissasArrayName','abscissasarray','str',1,'','name of the array where centerline abscissas have to be stored'],
            ['NormalsArrayName','normalsarray','str',1,'','name of the array where parallel transport normals to the centerlines have to be stored']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['AbscissasArrayName','abscissasarray','str',1,'','name of the array where centerline abscissas are stored'],
              ['NormalsArrayName','normalsarray','str',1,'','name of the array where parallel transport normals to the centerlines are stored']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineAttributes = vtkvmtk.vtkvmtkCenterlineAttributesFilter()
        centerlineAttributes.SetInputData(self.Centerlines)
        centerlineAttributes.SetAbscissasArrayName(self.AbscissasArrayName)
        centerlineAttributes.SetParallelTransportNormalsArrayName(self.NormalsArrayName)
        centerlineAttributes.Update()

        self.Centerlines = centerlineAttributes.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
