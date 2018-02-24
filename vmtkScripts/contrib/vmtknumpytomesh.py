#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknumpytomesh.py,v $
## Language:  Python
## Date:      FEB 23, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtkmeshtonumpy module, numpy is not installed')


class vmtkNumpyToMesh(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ArrayDict = None

        self.SetScriptName('vmtkNumpyToMesh')
        self.SetScriptDoc('Takes a nested python dictionary containing numpy arrays specifying Points, PointData, Cells,'
                           'CellData, and CellPointIds describing connectivity and returns a VMTK mesh (VTK Unstructured Grid) object ')
        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input array dictionary','vmtknumpyreader']])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']])


    
    def Execute(self):

        # grid data is the base class all cells, points, cell data, point data will be inserted into
        gridData = vtk.vtkUnstructuredGrid()


        print('converting points')
        points = vtk.vtkPoints()
        for xyzPoint in self.ArrayDict['Points']:
            points.InsertNextPoint(xyzPoint)
        gridData.SetPoints(points) # insert points (now a vtkPoints object) into gridData


        # cellArray defines cell type and cellPointIds
        cellArray = vtk.vtkCellArray()
        for cellId in self.ArrayDict['Cells']['CellPointIds']:
            numberOfCellPoints = cellId.size
            cellArray.InsertNextCell(numberOfCellPoints)
            for cellPoint in cellId:
                cellArray.InsertCellPoint(cellPoint)

        cellArray.SetTraversalLocation(cellArray.GetInsertLocation(0) + 1) 
        gridData.SetCells(self.ArrayDict['Cells']['CellTypes'], cellArray)

        if 'PointData' in self.ArrayDict.keys():
            print('converting point data')
            for pointKey in self.ArrayDict['PointData'].keys():
                if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, float):
                    pointDataArray = vtk.vtkFloatArray()

                else:
                    for checkDt in [int, np.uint8, np.uint16, np.uint32, np.uint64]:
                        if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, checkDt):
                            pointDataArray = vtk.vtkIntArray()
                            break
                        else:
                            continue

                try:
                    pointDataComponents = self.ArrayDict['PointData'][pointKey].shape[1]
                except IndexError:
                    pointDataComponents = 1

                if pointDataArray:
                    pointDataArray.SetNumberOfComponents(pointDataComponents)
                    pointDataArray.SetName(pointKey)

                    if pointDataComponents == 1:
                        pointDataArray.SetNumberOfValues(self.ArrayDict['PointData'][pointKey].size)
                        for index, pointData in enumerate(self.ArrayDict['PointData'][pointKey]):
                            pointDataArray.SetValue(index, pointData)
                        gridData.GetPointData().SetActiveScalars(pointKey)
                        gridData.GetPointData().SetScalars(pointDataArray)

                    else:
                        for pointData in self.ArrayDict['PointData'][pointKey]:
                            pointDataArray.InsertNextTuple(pointData)
                        gridData.GetPointData().SetActiveVectors(pointKey)
                        gridData.GetPointData().SetVectors(pointDataArray)

        print('converting cell data')
        for cellKey in self.ArrayDict['CellData'].keys():

            if cellKey == 'CellPointIds':
                cellDataArray = vtk.vtkCellArray()
                cellDataTypes = []
                for cellId in self.ArrayDict['CellData']['CellPointIds']:
                    numberOfCellPoints = cellId.size
                    cellDataArray.InsertNextCell(numberOfCellPoints)
                    for cellPoint in cellId:
                        cellDataArray.InsertCellPoint(cellPoint)
                        cellDataTypes.append(int)

                gridData.SetCells(cellDataTypes, cellDataArray)

            else:
                if np.issubdtype(self.ArrayDict['CellData'][cellKey].dtype, float):
                    cellDataArray = vtk.vtkFloatArray()
                if np.issubdtype(self.ArrayDict['CellData'][cellKey].dtype, int):
                    cellDataArray = vtk.vtkIntArray()

                try:
                    cellDataComponents = self.ArrayDict['CellData'][cellKey].shape[1]
                except IndexError:
                    cellDataComponents = 1

                cellDataArray.SetNumberOfComponents(cellDataComponents)
                cellDataArray.SetName(cellKey)

                if cellDataComponents == 1:
                    cellDataArray.SetNumberOfValues(self.ArrayDict['CellData'][cellKey].size)
                    for index, cellData in enumerate(self.ArrayDict['CellData'][cellKey]):
                        cellDataArray.SetValue(index, cellData)
                    gridData.GetCellData().SetActiveScalars(cellKey)
                    gridData.GetCellData().SetScalars(cellDataArray)

                else:
                    for cellData in self.self.ArrayDict['CellData'][cellKey]:
                        pointDataArray.InsertNextTuple(cellData)
                    gridData.GetCellData().SetActiveVectors(cellKey)
                    gridData.GetCellData().SetVectors(cellDataArray)

        self.Mesh = gridData

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()

        