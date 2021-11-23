## Module:    $RCSfile: vmtknumpytocenterlines.py,v $
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

from __future__ import absolute_import  # NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
from vtk.numpy_interface import dataset_adapter as dsa

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtknumpytosurface module, numpy is not installed')


class vmtkNumpyToCenterlines(pypes.pypeScript):
    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ArrayDict = None

        self.SetScriptName('vmtkNumpyToCenterlines')
        self.SetScriptDoc('Takes a nested python dictionary containing numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data specifying line connectivity'
                          'and returns a VTK centerlines vtkPolyData file')
        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input array dictionary','vmtknumpyreader']])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def Execute(self):

        polyData = vtk.vtkPolyData()
        wpPolyData = dsa.WrapDataObject(polyData)

        points = self.ArrayDict['Points']
        vtkPoints = dsa.numpyTovtkDataArray(points, 'Points')
        wpPolyData.Points = vtkPoints

        self.PrintLog('converting point data')
        for pointDataKey in self.ArrayDict['PointData'].keys():
            pointDataItem = self.ArrayDict['PointData'][pointDataKey]
            wpPolyData.PointData.append(pointDataItem, name=pointDataKey)

        self.PrintLog('converting cell data')
        for cellKey in self.ArrayDict['CellData'].keys():
            if cellKey == 'CellPointIds':
                cellDataArray = vtk.vtkCellArray()
                for cellId in self.ArrayDict['CellData']['CellPointIds']:
                    numberOfCellPoints = cellId.size
                    cellDataArray.InsertNextCell(numberOfCellPoints)
                    for cellPoint in cellId:
                        cellDataArray.InsertCellPoint(cellPoint)
                wpPolyData.VTKObject.SetLines(cellDataArray)
            else:
                cellDataItem = self.ArrayDict['CellData'][cellKey]
                wpPolyData.CellData.append(cellDataItem, name=cellKey)

        self.Centerlines = wpPolyData.VTKObject


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
