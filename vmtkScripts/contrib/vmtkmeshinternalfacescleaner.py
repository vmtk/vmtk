#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshinternalfacescleaner.py,v $
## Language:  Python
## Date:      $Date: 2021/09/15 09:53:14 $
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


class vmtkMeshInternalFacesCleaner(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.CleanedCells = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.PlaceHolder = 9999

        self.SetScriptName('vmtkmeshinternalfacescleaner')
        self.SetScriptDoc('clean from an input vtkUnstructuredGrid all the 2D cells that do not lay on the boundary')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps have to be stored'],
            ['PlaceHolder','placeholder','int',1,'','the placeholder entity id assigned to the cells to be cleaned']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['CleanedCells','ocleaned','vtkUnstructuredGrid',1,'','the output mesh containing the cleaned cells','vmtkmeshwriter'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'','name of the array where the id of the caps are stored']
            ])

    def Execute(self):
        from vmtk import vmtkscripts
        from vmtk import vmtkcontribscripts

        cellEntityIdsArray = self.Mesh.GetCellData().GetArray(self.CellEntityIdsArrayName)

        if cellEntityIdsArray == None:
            cellEntityIdsArray = vtk.vtkIntArray()
            cellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
            cellEntityIdsArray.SetNumberOfTuples(self.Mesh.GetNumberOfCells())
            cellEntityIdsArray.FillComponent(0,1)
            self.Mesh.GetCellData().AddArray(cellEntityIdsArray)

        triaCellType = vtk.vtkTriangle().GetCellType()
        quadCellType = vtk.vtkQuad().GetCellType()

        triaCellIdArray = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(triaCellType,triaCellIdArray)

        quadCellIdArray = vtk.vtkIdTypeArray()
        self.Mesh.GetIdsOfCellsOfType(quadCellType,quadCellIdArray)

        def tagsInternalCells(cellType2DIdArray):
            countCleanedCells = 0
            for i in range(cellType2DIdArray.GetNumberOfTuples()):
                cell2DId = cellType2DIdArray.GetValue(i)
                cell2DPointIds = self.Mesh.GetCell(cell2DId).GetPointIds()
                neighborCellIds = vtk.vtkIdList()
                self.Mesh.GetCellNeighbors(cell2DId,cell2DPointIds,neighborCellIds)
                count3DNeighbors = neighborCellIds.GetNumberOfIds()
                if count3DNeighbors > 1:
                    countCleanedCells += 1
                    cellEntityIdsArray.SetValue(cell2DId,self.PlaceHolder)

            if countCleanedCells>0:
                print("\tNumber of cleaned cells: ",countCleanedCells)

        tagsInternalCells(triaCellIdArray)
        tagsInternalCells(quadCellIdArray)

        extract = vmtkcontribscripts.vmtkEntityExtractor()
        extract.Mesh = self.Mesh
        extract.CellEntityIdsArrayName = self.CellEntityIdsArrayName
        extract.EntityIds = self.PlaceHolder
        extract.Invert = 1
        extract.ConvertToInt = 1
        extract.Execute()
        self.Mesh = extract.Mesh
        self.CleanedCells = extract.DeletedMesh


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
