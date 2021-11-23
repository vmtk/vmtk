#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshvolume.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
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


class vmtkMeshVolume(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.Volume = 0.0

        self.SetScriptName('vmtkmeshvolume')
        self.SetScriptDoc('computes the volume of a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkPolyData',1,'','the input mesh','vmtkmeshreader']
            ])
        self.SetOutputMembers([
            ['Volume','volume','float',1,'']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        tetrahedralizeFilter = vtk.vtkDataSetTriangleFilter()
        tetrahedralizeFilter.SetInputData(self.Mesh)
        tetrahedralizeFilter.TetrahedraOnlyOn()
        tetrahedralizeFilter.Update()

        self.Mesh = tetrahedralizeFilter.GetOutput()

        self.Volume = 0.0
        cellPoints = vtk.vtkIdList()
        for i in range(self.Mesh.GetNumberOfCells()):
            self.Mesh.GetCellPoints(i,cellPoints)
            point0 = self.Mesh.GetPoint(cellPoints.GetId(0))
            point1 = self.Mesh.GetPoint(cellPoints.GetId(1))
            point2 = self.Mesh.GetPoint(cellPoints.GetId(2))
            point3 = self.Mesh.GetPoint(cellPoints.GetId(3))
            self.Volume += abs(vtk.vtkTetra.ComputeVolume(point0,point1,point2,point3))


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
