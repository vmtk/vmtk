#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinestonumpy.py,v $
## Language:  Python
## Date:      June 10, 2017
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo
##       The Jacobs Institute

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
    raise ImportError('Unable to Import vmtkcenterlinestonumpy module, numpy is not installed')


# convenience class for nested dictionaries
class vividict(dict):
    def __missing__(self, key):
        value = self[key] = type(self)()
        return value


class vmtkCenterlinesToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.ConvertCellToPoint = 0
        self.ArrayDict = vividict()

        self.SetScriptName('vmtkCenterlinesToNumpy')
        self.SetScriptDoc('Takes a VTK centerlines vtkPolyData file and returns a nested python dictionary containing numpy'
                          'arrays specifying vertex points, associated scalar data, and cell data yielding connectivity')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['ConvertCellToPoint','celltopoint','bool',1,'','convert cell data to point data']])
        self.SetOutputMembers([
            ['ArrayDict','o','dict',1,'','the output dictionary','vmtknumpywriter']])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        if self.ConvertCellToPoint == 1:
            self.PrintLog('converting cell data to point data')
            try:
                from vmtk import vmtksurfacecelldatatopointdata
            except ImportError:
                raise ImportError('unable to import vmtksurfacecelldata to point data module')

            centerlinesCellToPoint = vmtksurfacecelldatatopointdata.vmtkSurfaceCellDataToPointData()
            centerlinesCellToPoint.Surface = self.Centerlines
            centerlinesCellToPoint.Execute()
            self.PrintLog('wrapping vtkPolyData object')
            clWrapper = dsa.WrapDataObject(centerlinesCellToPoint.Surface)

        else:
            self.PrintLog('wrapping vtkPolyData object')
            clWrapper = dsa.WrapDataObject(self.Centerlines)

            self.PrintLog('converting cell data: ')
            for cellKey in clWrapper.CellData.keys():
                self.PrintLog(cellKey)
                self.ArrayDict['CellData'][cellKey] = np.array(clWrapper.CellData.GetArray(cellKey))

        self.PrintLog('converting points')
        self.ArrayDict['Points'] = np.array(clWrapper.Points)

        self.PrintLog('converting point data: ')
        for pointKey in clWrapper.PointData.keys():
            self.PrintLog(pointKey)
            self.ArrayDict['PointData'][pointKey] = np.array(clWrapper.PointData.GetArray(pointKey))

        self.PrintLog('converting cell connectivity list')
        cellPointIdsList = []
        numberOfCells = clWrapper.VTKObject.GetNumberOfCells()
        for cellId in range(numberOfCells):
            cell = clWrapper.VTKObject.GetCell(cellId)
            numberOfPointsPerCell = cell.GetNumberOfPoints()
            cellArray = np.zeros(shape=numberOfPointsPerCell, dtype=np.int32)
            for point in range(numberOfPointsPerCell):
                cellArray[point] = cell.GetPointId(point)
            cellPointIdsList.append(cellArray)

        self.ArrayDict['CellData']['CellPointIds'] = cellPointIdsList


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
