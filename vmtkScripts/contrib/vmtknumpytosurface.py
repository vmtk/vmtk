## Module:    $RCSfile: vmtknumpytosurface.py,v $
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
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtknumpytosurface module, numpy is not installed')


class vmtkNumpyToSurface(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.ArrayDict = None

        self.SetScriptName('vmtkNumpyToSurface')
        self.SetScriptDoc('Takes a nested python dictionary containing numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data specifying triangle connectivity'
                          'and returns a VTK triangulated surface vtkPolyData file')
        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input array dictionary','vmtknumpyreader']])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def Execute(self):

        polyData = vtk.vtkPolyData()

        self.PrintLog('converting points')
        points = vtk.vtkPoints()
        for xyzPoint in self.ArrayDict['Points']:
            points.InsertNextPoint(xyzPoint)

        polyData.SetPoints(points)

        self.PrintLog('converting numpy array to surface')
        for pointKey in self.ArrayDict['PointData'].keys():

            if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, np.floating):
                pointDataArray = vtk.vtkFloatArray()
            else:
                for checkDt in [np.integer, np.uint8, np.uint16, np.uint32, np.uint64]:
                    if np.issubdtype(self.ArrayDict['PointData'][pointKey].dtype, checkDt):
                        pointDataArray = vtk.vtkIntArray()
                        break
                    else:
                        continue

            try:
                pointDataComponents = self.ArrayDict['PointData'][pointKey].shape[1]
            except IndexError:
                pointDataComponents = 1

            pointDataArray.SetNumberOfComponents(pointDataComponents)
            pointDataArray.SetName(pointKey)

            if pointDataComponents == 1:
                pointDataArray.SetNumberOfValues(self.ArrayDict['PointData'][pointKey].size)
                for index, pointData in enumerate(self.ArrayDict['PointData'][pointKey]):
                    pointDataArray.SetValue(index, pointData)
                polyData.GetPointData().SetActiveScalars(pointKey)
                polyData.GetPointData().SetScalars(pointDataArray)
            else:
                for pointData in self.ArrayDict['PointData'][pointKey]:
                    pointDataArray.InsertNextTuple(pointData)
                polyData.GetPointData().SetActiveVectors(pointKey)
                polyData.GetPointData().SetVectors(pointDataArray)

        def mkVtkIdList(it):
            vil = vtk.vtkIdList()
            for i in it:
                vil.InsertNextId(int(i))
            return vil

        for cellKey in self.ArrayDict['CellData'].keys():

            if cellKey == 'CellPointIds':
                cellDataArray = vtk.vtkCellArray()
                for cellId in self.ArrayDict['CellData'][cellKey]:
                    if type(cellId) is np.ndarray:
                        cellDataArray.InsertNextCell(mkVtkIdList(cellId))
                    else:
                        for cell in cellId:
                            cellDataArray.InsertNextCell(mkVtkIdList(cell))
                polyData.SetPolys(cellDataArray)

            else:

                if np.issubdtype(self.ArrayDict['CellData'][cellKey].dtype, np.floating):
                    cellDataArray = vtk.vtkFloatArray()
                if np.issubdtype(self.ArrayDict['CellData'][cellKey].dtype, np.integer):
                    cellDataArray = vtk.vtkIntArray()

                try:
                    cellDataComponents = self.ArrayDict['CellData'][cellKey].shape[1]
                except IndexError:
                    cellDataComponents = 1

                cellDataArray.SetNumberOfComponents(pointDataComponents)
                cellDataArray.SetName(cellKey)

                if cellDataComponents == 1:
                    cellDataArray.SetNumberOfValues(self.ArrayDict['CellData'][cellKey].size)
                    for index, cellData in enumerate(self.ArrayDict['CellData'][cellKey]):
                        cellDataArray.SetValue(index, cellData)
                    polyData.GetCellData().SetActiveScalars(cellKey)
                    polyData.GetCellData().SetScalars(cellDataArray)

                else:
                    for cellData in self.ArrayDict['CellData'][cellKey]:
                        pointDataArray.InsertNextTuple(cellData)
                    polyData.GetCellData().SetActiveVectors(cellKey)
                    polyData.GetCellData().SetVectors(cellDataArray)

        self.Surface = polyData


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
