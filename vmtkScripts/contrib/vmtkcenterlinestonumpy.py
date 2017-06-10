#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinestonumpy.py,v $
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

class vmtkCenterlinesToNumpy(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.OutputVariableName = None

        self.CenterlinesArrayDict = vividict()

        self.SetScriptName('vmtkCenterlinesToNumpy')
        self.SetScriptDoc('Takes a VTK centerlines vtkPolyData file (optionally containing point data scalar '
                          'arrays or cell data scalar arrays) and returns a nested python dictionary containing numpy '
                          'arrays specifying vertex points, associated scalar data, and cell data yielding connectivity')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['OutputVariableName','outputvariablename','str',1,'','variable name to store the dictionary']])
        self.SetOutputMembers([
            ['CenterlinesArrayDict','o','dict',1]
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        # if self.OutputVariableName == None:
        #     self.PrintError('Error: No variable name specified for output dictionary')

        self.PrintLog('converting any cell data to point data')
        try:
            from vmtk import vmtksurfacecelldatatopointdata
        except ImportError:
            raise ImportError('unable to import vmtksurfacecelldata to point data module')
        centerlinesCellToPoint = vmtksurfacecelldatatopointdata.vmtkSurfaceCellDataToPointData()
        centerlinesCellToPoint.Surface = self.Centerlines
        centerlinesCellToPoint.Execute()

        self.PrintLog('wrapping vtkPolyData object')
        wrappedCenterlines = dsa.WrapDataObject(centerlinesCellToPoint.Surface)

        self.PrintLog('writing vertex points to dictionary')
        self.CenterlinesArrayDict['Points'] = np.array(wrappedCenterlines.Points)

        self.PrintLog('writing point data scalars to dictionary')
        pointDataKeys = wrappedCenterlines.PointData.keys()
        for key in pointDataKeys:
            self.CenterlinesArrayDict['PointData'][key] = np.array(wrappedCenterlines.PointData.GetArray(key))

        self.PrintLog('writing cell data to dictionary')
        numberOfCells = centerlinesCellToPoint.Surface.GetNumberOfCells()
        for cellId in range(numberOfCells):
            cell = centerlinesCellToPoint.Surface.GetCell(cellId)
            numberOfPointsPerCell = cell.GetNumberOfPoints()

            cellArray = np.zeros(shape=numberOfPointsPerCell, dtype=np.int32)
            for point in range(numberOfPointsPerCell):
                cellArray[point] = cell.GetPointId(point)

            self.CenterlinesArrayDict['CellData']['CellPointIds'][str(cellId)] = cellArray


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()