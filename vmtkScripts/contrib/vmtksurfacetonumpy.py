#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetonumpy.py,v $
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
    raise ImportError('Unable to Import vmtksurfacetonumpy module, numpy is not installed')


# convenience class for nested dictionaries
class vividict(dict):
    def __missing__(self, key):
        value = self[key] = type(self)()
        return value


class vmtkSurfaceToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ConvertCellToPoint = 0
        self.ArrayDict = vividict()

        self.SetScriptName('vmtkSurfaceToNumpy')
        self.SetScriptDoc('Takes a VTK triangulated surface vtkPolyData file (optionally containing point data scalar '
                          'arrays) and returns a nested python dictionary containing numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data yielding triangle connectivity')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ConvertCellToPoint', 'celltopoint', 'bool', 1, '', 'convert cell data to point data']])
        self.SetOutputMembers([
            ['ArrayDict','o','dict',1,'','the output dictionary','vmtknumpywriter']])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.ConvertCellToPoint == 1:

            self.PrintLog('converting any cell data to point data')
            try:
                from vmtk import vmtksurfacecelldatatopointdata
            except ImportError:
                raise ImportError('unable to import vmtksurfacecelldatatopointdata module')
            surfaceCellToPoint = vmtksurfacecelldatatopointdata.vmtkSurfaceCellDataToPointData()
            surfaceCellToPoint.Surface = self.Surface
            surfaceCellToPoint.Execute()

            self.PrintLog('wrapping vtkPolyData object')
            surfWrapper = dsa.WrapDataObject(surfaceCellToPoint.Surface)

        else:
            self.PrintLog('wrapping vtkPolyData object')
            surfWrapper = dsa.WrapDataObject(self.Surface)

            self.PrintLog('converting cell data: ')
            for cellKey in surfWrapper.CellData.keys():
                self.PrintLog(cellKey)
                self.ArrayDict['CellData'][cellKey] = np.array(surfWrapper.CellData.GetArray(cellKey))

        self.PrintLog('converting points')
        self.ArrayDict['Points'] = np.array(surfWrapper.Points)

        self.PrintLog('converting point data: ')
        for pointKey in surfWrapper.PointData.keys():
            self.PrintLog(pointKey)
            self.ArrayDict['PointData'][pointKey] = np.array(surfWrapper.PointData.GetArray(pointKey))

        if not surfWrapper.PointData.keys():
            self.ArrayDict['PointData'] = {}

        self.PrintLog('converting cell connectivity list')
        numberOfCells = surfWrapper.VTKObject.GetNumberOfCells()
        numberOfPointsPerCell = surfWrapper.VTKObject.GetCell(0).GetNumberOfPoints()

        cellArray = np.zeros(shape=(numberOfCells, numberOfPointsPerCell), dtype=np.int32)
        it = np.nditer(cellArray, flags=['multi_index'], op_flags=['readwrite'])

        # this is an efficient ndarray iterator method. the loop "for x in it" pulls an element out of
        # the cellArray iterator (it) and with the elipses syntax (x[...]) writes the point id.
        # this is equivalent to writing
        # for cellId in range(numberOfCells):
        #     cell = surfWrapper.VTKObject.GetCell(cellId)
        #     for point in range(numberOfPointsPerCell):
        #         cellArray[cellId, point] = cell.GetPointId(point)

        for x in it:
            x[...] = surfWrapper.VTKObject.GetCell(it.multi_index[0]).GetPointId(it.multi_index[1])

        self.ArrayDict['CellData']['CellPointIds'] = cellArray


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
