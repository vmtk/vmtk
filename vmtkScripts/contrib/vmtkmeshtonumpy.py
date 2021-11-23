#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshtonumpy.py,v $
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

# convenience class for nested dictionaries


class vividict(dict):
    def __missing__(self, key):
        value = self[key] = type(self)()
        return value


class vmtkMeshToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.ReturnCellTypesAsStrings = 1
        self.StructureCellsAsList = 0
        self.ArrayDict = vividict()

        self.SetScriptName('vmtkMeshToNumpy')
        self.SetScriptDoc('Takes a VMTK mesh (VTK Unstructured Grid) object nested python dictionary containing numpy'
                          'arrays specifying Points, PointData, Cells, CellData, and CellPointIds describing connectivity')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ReturnCellTypesAsStrings','typestrings','bool',1,'','return a mapping of the vtk cell type class names to the cell type object IDs'],
            ['StructureCellsAsList','structure','bool',0,'','enable to return a list of numpy arrays where each element is an individual cell. Note: This is a computationally intensive operation which may fill memory, and which will drastically increase disk space used if data is saved. Not recommended for large data sets.']])
        self.SetOutputMembers([
            ['ArrayDict','o','dict',1,'','the output dictionary','vmtknumpywriter']])

    def _ConvertFlatCellsArrayToList(self, cells, cellLocations):
        '''convert a flat array defining cells into a list of numpy arrays which each define a cell
        
        this function is the inverse of vmtk.numpytomesh._ConvertListToFlatCellsArray(cellPointIdsList)
        
        arguments: 
            - cells: 1D array of format [npointsCell1, cell1PointId_1, .., cell1PointId_npointsCell1, 
                                        npointsCell2, cell2PointId_1, .., cell2PointId_npointsCell2, 
                                        ... 
                                        npointsCell(nCells), cell(nCells)PointId_1, .. cell(nCells)PointId_npointsCell(nCells)]
                                        
            - cellLocations: flat array of size = nCells. each element in the array defines starts a new cell 
                            (a location of npointCellFoo) in the cells array
        
        returns:
            - cellPointIdsList: list of numpy arrays (in same order defined in cells), where each array contains 
                                the cellPointIds for that specific cell. Note: array lengths are not constant, 
                                each cell can have a different number of constituent points.
        '''

        splitArrays = np.split(cells, cellLocations[1:]) # start first split after end of Cell1, at nPointsCell2

        cellPointIdsList = []
        for subArray in splitArrays:
            cellPointIdsList.append(subArray[1:]) # get rid of npointsCell(foo), only keep cellPointIds

        return cellPointIdsList

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        wrappedData = dsa.WrapDataObject(self.Mesh)

        # (npoints, 3) array of xyz coordinates. values in Cells index to the
        # rows of this array (Cells.max() = Points.shape[0] - 1)
        points = np.array(wrappedData.Points)
        self.ArrayDict['Points'] = points

        if self.StructureCellsAsList:
            cellPointIdsList = self._ConvertFlatCellsArrayToList(np.array(wrappedData.Cells),
                                                                np.array(wrappedData.CellLocations))
            self.ArrayDict['Cells']['CellPointIds'] = cellPointIdsList

        else:
            # flat list of locations which index the beginning of a cell in the
            # Cell array (same size as CellTypes and CellEntityIds)
            cellLocations = np.array(wrappedData.CellLocations)
            self.ArrayDict['Cells']['CellLocations'] = cellLocations

            # flat array which defines the npoints/cell and indexes of rows
            # in Points array which define each cell's XYZ locations
            cells = np.array(wrappedData.Cells)
            self.ArrayDict['Cells']['CellPointIds'] = cells

        # flat array of shape == cellEntityIds == cellLocations which defines
        # the intiger descriptor of the VTK_CELL_TYPE for each cell in cells
        cellTypes = np.array(wrappedData.CellTypes)
        self.ArrayDict['Cells']['CellTypes'] = cellTypes

        if self.ReturnCellTypesAsStrings == 1:
            typeDict = vividict()
            uniqueCellTypes = np.unique(cellTypes)
            for cellType in uniqueCellTypes:
                typeDict[vtk.vtkCellTypes.GetClassNameFromTypeId(cellType)] = np.array([cellType])
            self.ArrayDict['Cells']['CellTypesAsStrings'] = typeDict

        for cellDataKey in wrappedData.CellData.keys():
            cellData = wrappedData.CellData.GetArray(cellDataKey)
            self.ArrayDict['CellData'][cellDataKey] = cellData

        for pointDataKey in wrappedData.PointData.keys():
            pointData = wrappedData.PointData.GetArray(pointDataKey)
            self.ArrayDict['PointData'][pointDataKey] = pointData


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
