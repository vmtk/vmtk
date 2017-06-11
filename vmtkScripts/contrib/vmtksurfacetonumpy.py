#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacetonumpy.py,v $
## Language:  Python
## Date:      June 10, 2017
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

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


# convenience class for nested disctionaries
class vividict(dict):
    def __missing__(self, key):
        value = self[key] = type(self)()
        return value

class vmtkSurfaceToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None

        self.ArrayDict = vividict()

        self.SetScriptName('vmtkSurfaceToNumpy')
        self.SetScriptDoc('Takes a VTK triangulated surface vtkPolyData file (optionally containing point data scalar '
                          'arrays) and returns a nested python dictionary containing numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data yielding triangle connectivity')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader']])
        self.SetOutputMembers([
            ['ArrayDict','o','dict',1]])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        self.PrintLog('converting any cell data to point data')
        try:
            from vmtk import vmtksurfacecelldatatopointdata
        except ImportError:
            raise ImportError('unable to import vmtksurfacecelldatatopointdata module')
        surfaceCellToPoint = vmtksurfacecelldatatopointdata.vmtkSurfaceCellDataToPointData()
        surfaceCellToPoint.Surface = self.Surface
        surfaceCellToPoint.Execute()

        self.PrintLog('wrapping vtkPolyData object')
        wrappedSurface = dsa.WrapDataObject(surfaceCellToPoint.Surface)

        self.PrintLog('writing vertex points to dictionary')
        self.ArrayDict['Points'] = np.array(wrappedSurface.Points)

        self.PrintLog('writing point data scalars to dictionary')
        pointDataKeys = wrappedSurface.PointData.keys()
        for key in pointDataKeys:
            self.ArrayDict['PointData'][key] = np.array(wrappedSurface.PointData.GetArray(key))

        self.PrintLog('writing cell data to dictionary')
        numberOfCells = surfaceCellToPoint.Surface.GetNumberOfCells()
        numberOfPointsPerCell = surfaceCellToPoint.Surface.GetCell(0).GetNumberOfPoints()

        cellArray = np.zeros(shape=(numberOfCells, numberOfPointsPerCell), dtype=np.int32)
        for cellId in range(numberOfCells):
            cell = surfaceCellToPoint.Surface.GetCell(cellId)
            for point in range(numberOfPointsPerCell):
                cellArray[cellId, point] = cell.GetPointId(point)

        self.ArrayDict['CellData']['CellPointIds'] = cellArray


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()