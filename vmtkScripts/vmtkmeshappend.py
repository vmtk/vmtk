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


class vmtkMeshAppend(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.InputFileNames = None
        self.MergePoints = 1
        self.Tolerance = 0.0 # vtk >= 9.0
        self.Mesh = None

        self.SetScriptName('vmtkmeshappend')
        self.SetScriptDoc('merge two or more meshes into a unique one')
        self.SetInputMembers([
            ['InputFileNames','ifiles','str',-1,'','the list of the files of the input meshes to be merged'],
            ['MergePoints','mergepoints','bool',1,'','toggle merging points while appending input meshes']
            # ['Tolerance','tolerance','float',1,'(0.0,)','tolerance for merging points'] # vtk >= 9.0
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])



    def Execute(self):
        from vmtk import vmtkscripts

        append = vtk.vtkAppendFilter()
        append.SetMergePoints(self.MergePoints)
        # append.SetTolerance(self.Tolerance) # vtk >= 9.0

        for filename in self.InputFileNames:
            reader = vmtkscripts.vmtkMeshReader()
            reader.InputFileName = filename
            reader.Execute()
            append.AddInputData(reader.Mesh)

        append.Update()

        self.Mesh = append.GetOutput()



if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
