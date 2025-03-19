#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceappend.py,v $
## Language:  Python
## Date:      $Date: 2020/10/22 10:28:39 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
import numpy as np

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkEntityList(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Surface = None
        self.Input = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ConvertToInt = 0

        self.EntityIds = []
        self.VolumeEntityIds = []
        self.SurfaceEntityIds = []
        self.LineEntityIds = []


        self.SetScriptName('vmtkentitylist')
        self.SetScriptDoc('list all the entity ids of an input mesh or surface')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh', 'vmtkmeshreader'],
            ['Surface','isurface','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'',''],
            ['ConvertToInt','converttoint','bool',1,'','toggle converting the input CellEntityIdsArray into a vtkIntArray (if it is not already of this type)']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['Surface','osurface','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['EntityIds','ids','int',-1,'','the complete list of ids'],
            ['VolumeEntityIds','volumeids','int',-1,'','the list of ids of the 3D elements (e.g. tetrahedra, hexahedra)'],
            ['SurfaceEntityIds','surfaceids','int',-1,'','the list of ids of the 2D elements (e.g. triangles, quads)'],
            ['LineEntityIds','lineids','int',-1,'','the list of ids of the 1D elements (e.g. lines)'],
            ])


    def Execute(self):

        if self.Surface == None and self.Mesh == None:
            self.PrintError('Error: No Surface or Mesh.')
        if self.Surface != None and self.Mesh != None:
            self.PrintError('Error: Both Surface and Mesh, expecting only one.')

        self.Input = self.Surface or self.Mesh

        self.CellEntityIdsArray = self.Input.GetCellData().GetArray(self.CellEntityIdsArrayName)
        if self.CellEntityIdsArray == None:
            self.PrintError('Error: CellEntityIdsArray with the given name does not exist in the input surface/mesh')

        if self.CellEntityIdsArray.GetNumberOfComponents()!=1:
            self.PrintError('Error: CellEntityIdsArray must have a single component')

        if not self.ConvertToInt and self.CellEntityIdsArray.GetDataType()!=6: # 6 = int
            self.PrintError('Error: CellEntityIdsArray must be a vtkIntArray (you can activate ConvertToInt option to convert it)')

        if self.ConvertToInt and self.CellEntityIdsArray.GetDataType()!=6: # 6 = int
            idsArray = vtk.vtkIntArray()
            idsArray.DeepCopy(self.Input.GetCellData().GetArray(self.CellEntityIdsArrayName))
            self.Input.GetCellData().RemoveArray(self.CellEntityIdsArrayName)
            self.Input.GetCellData().AddArray(idsArray)
            self.CellEntityIdsArray = self.Input.GetCellData().GetArray(self.CellEntityIdsArrayName)

        # only linear elements
        volumeTypes = (vtk.VTK_TETRA,vtk.VTK_VOXEL,vtk.VTK_HEXAHEDRON,vtk.VTK_WEDGE,vtk.VTK_PYRAMID,vtk.VTK_PENTAGONAL_PRISM,vtk.VTK_HEXAGONAL_PRISM)
        surfaceTypes = (vtk.VTK_TRIANGLE,vtk.VTK_TRIANGLE_STRIP,vtk.VTK_POLYGON,vtk.VTK_PIXEL,vtk.VTK_QUAD)
        lineTypes = (vtk.VTK_LINE,vtk.VTK_POLY_LINE)

        self.EntityIds = set()
        self.VolumeEntityIds = set()
        self.SurfaceEntityIds = set()
        self.LineEntityIds = set()

        for i in range(self.Input.GetNumberOfCells()):
            currId = self.CellEntityIdsArray.GetComponent(i,0)
            self.EntityIds.add(currId)
            if self.Input.GetCell(i).GetCellType() in volumeTypes:
                self.VolumeEntityIds.add(currId)
            elif self.Input.GetCell(i).GetCellType() in surfaceTypes:
                self.SurfaceEntityIds.add(currId)
            elif self.Input.GetCell(i).GetCellType() in lineTypes:
                self.LineEntityIds.add(currId)

        def toIntList(aList):
            return list(map(int,aList))

        self.EntityIds = toIntList(sorted(self.EntityIds))
        self.VolumeEntityIds = toIntList(sorted(self.VolumeEntityIds))
        self.SurfaceEntityIds = toIntList(sorted(self.SurfaceEntityIds))
        self.LineEntityIds = toIntList(sorted(self.LineEntityIds))



if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

