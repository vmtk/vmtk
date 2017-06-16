## Module:    $RCSfile: vmtknumpytocenterlines.py,v $
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

from __future__ import absolute_import  # NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

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
        self.SetScriptDoc('Takes a nested python dictionary containg numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data specifying line connectivity'
                          'and returns a VTK centerlines vtkPolyData file')
        self.SetInputMembers([
            ['ArrayDict', 'i', 'dict', 1, '', 'the input dictionary']])
        self.SetOutputMembers([
            ['Centerlines', 'o', 'vtkPolyData', 1, '', 'the output surface', 'vmtksurfacewriter']])

    def Execute(self):

        polyData = vtk.vtkPolyData()

        points = vtk.vtkPoints()
        for xyzPoint in self.ArrayDict['Points']:
            points.InsertNextPoint(xyzPoint)

        polyData.SetPoints(points)

        self.PrintLog('converting numpy array to centerlines')
        pointDataKeys = self.ArrayDict['PointData'].keys()
        for key in pointDataKeys:

            if np.issubdtype(self.ArrayDict['PointData'][key].dtype, float):
                pointDataArray = vtk.vtkFloatArray()
            if np.issubdtype(self.ArrayDict['PointData'][key].dtype, int):
                pointDataArray = vtk.vtkIntArray()

            try:
                pointDataComponents = self.ArrayDict['PointData'][key].shape[1]
            except IndexError:
                pointDataComponents = 1

            pointDataArray.SetNumberOfComponents(pointDataComponents)
            pointDataArray.SetName(key)

            if pointDataComponents == 1:
                pointDataArray.SetNumberOfValues(self.ArrayDict['PointData'][key].size)
                for index, pointData in enumerate(self.ArrayDict['PointData'][key]):
                    pointDataArray.SetValue(index, pointData)
                polyData.GetPointData().SetActiveScalars(key)
                polyData.GetPointData().SetScalars(pointDataArray)

            else:
                pointDataArray.SetNumberOfValues(self.ArrayDict['PointData']['key'].shape[0])
                for index, pointData in enumerate(self.ArrayDict['PointData'][key]):
                    pointDataArray.SetTupleValue(index, pointData)
                polyData.GetPointData().SetActiveVectors(key)
                polyData.GetPointData().SetVectors(pointDataArray)

        cellDataArray = vtk.vtkCellArray()
        numberOfCells = len(self.ArrayDict['CellData']['CellPointIds'].keys())
        for cellId in range(numberOfCells):
            numberOfCellPoints = self.ArrayDict['CellData']['CellPointIds'][str(cellId)].size
            cellDataArray.InsertNextCell(numberOfCellPoints)
            for cellPoint in self.ArrayDict['CellData']['CellPointIds'][str(cellId)]:
                cellDataArray.InsertCellPoint(cellPoint)

        polyData.SetLines(cellDataArray)

        try:
            from vmtk import vmtksurfacepointdatatocelldata
        except ImportError:
            raise ImportError('unable to import vmtksurfacepointdatatocelldata module')

        pointToCell = vmtksurfacepointdatatocelldata.vmtkSurfacePointDataToCellData()
        pointToCell.Surface = polyData
        pointToCell.Execute()

        self.Centerlines = pointToCell.Surface


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()