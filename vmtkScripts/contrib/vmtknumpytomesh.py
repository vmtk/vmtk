#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknumpytomesh.py,v $
## Language:  Python
## Date:      FEB 23, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vtk.numpy_interface import dataset_adapter as dsa
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
        self.FlattenListOfCells = 0

        self.SetScriptName('vmtkNumpyToMesh')
        self.SetScriptDoc('Takes a nested python dictionary containing numpy arrays specifying Points, PointData, Cells,'
                           'CellData, and CellPointIds describing connectivity and returns a VMTK mesh (VTK Unstructured Grid) object ')
        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input array dictionary','vmtknumpyreader'],
            ['FlattenListOfCells','flatten','bool',0,'','enable to convert cells which are formated as a list of numpy arrays to the default flat structure']])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']])

    def _ConvertListToFlatCellsArray(self, cellPointIdsList):
        '''convert a list of numpy arrays defining each cell into a flat array defining cells. 
        
        This function is the inverse of vmtk.meshtonumpy._ConvertFlatCellsArrayToList(cells, cellLocations)
        
        arguments: 
            - cellPointIdsList: list of numpy arrays (in same order defined in cells), where each array contains 
                                the cellPointIds for that specific cell. Note: array lengths are not constant, 
                                each cell can have a different number of constituent points.
        
        returns:
        
            - cells: 1D array of format [npointsCell1, cell1PointId_1, .., cell1PointId_npointsCell1, 
                                        npointsCell2, cell2PointId_1, .., cell2PointId_npointsCell2, 
                                        ... 
                                        npointsCell(nCells), cell(nCells)PointId_1, .. cell(nCells)PointId_npointsCell(nCells)]

            - cellLocations: flat array of size = nCells. each element in the array defines starts a new cell 
                            (a location of npointCellFoo) in the cells array
        '''

        cellArrayList = []
        cellLocationsList = [np.array([0])]
        cellIndex = 0

        for cellPointIdArray in cellPointIdsList:
            numPointsInArray = cellPointIdArray.size
            cellArray = np.concatenate((np.array([numPointsInArray]), cellPointIdArray))
            cellArrayList.append(cellArray)

            cellIndex += cellArray.size
            cellLocationsList.append(np.array([cellIndex]))

        cellLocations = np.concatenate(cellLocationsList[:-1])
        cells = np.concatenate(cellArrayList)

        return cells, cellLocations

    def Execute(self):

        gridData = vtk.vtkUnstructuredGrid()
        gridData = dsa.WrapDataObject(gridData)

        gridData.SetPoints(self.ArrayDict['Points'])

        if self.FlattenListOfCells:
            cellPointIds, cellLocations = self._ConvertListToFlatCellsArray(self.ArrayDict['Cells']['CellPointIds'])
            gridData.SetCells(self.ArrayDict['Cells']['CellTypes'],
                              cellLocations, cellPointIds)

        else:
            gridData.SetCells(self.ArrayDict['Cells']['CellTypes'],
                            self.ArrayDict['Cells']['CellLocations'],
                            self.ArrayDict['Cells']['CellPointIds'])

        if 'PointData' in self.ArrayDict.keys():
            for pointKey, pointData in self.ArrayDict['PointData'].items():
                gridData.PointData.append(pointData, pointKey)

        if 'CellData' in self.ArrayDict.keys():
            for cellKey, cellData in self.ArrayDict['CellData'].items():
                gridData.CellData.append(cellData, cellKey)

        self.Mesh = gridData.VTKObject


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
