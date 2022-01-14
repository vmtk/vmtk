#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshappend.py,v $
## Language:  Python
## Date:      $Date: 2021/09/16 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkMeshCoords(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.SetScriptName('vmtkMeshcoords')
        self.SetScriptDoc('add coordinates as point data array on the surface')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input surface','vmtkmeshreader'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output surface','vmtkmeshwriter']
            ])

    def Execute(self):
        from vmtk import vmtkcontribscripts
        if self.Mesh == None:
            self.PrintError('Error: No input surface.')

        vmtkcontribscripts.vmtkSurfaceCoords().CreateCoords(self.Mesh)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
