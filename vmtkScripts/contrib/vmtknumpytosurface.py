## Module:    $RCSfile: vmtknumpytosurface.py,v $
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

        self.InputDict = None

        self.SetScriptName('vmtkNumpyToSurface')
        self.SetScriptDoc('Takes a nested python dictionary containg numpy arrays specifying vertex '
                          'points, associated scalar data, and cell data specifying triangle connectivity' 
                          'and returns a VTK triangulated surface vtkPolyData file')
        self.SetInputMembers([
            ['InputDict','i','dict',1,'','the input dictionary']])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def Execute(self):

        polyData = vtk.vtkPolyData()

        points = vtk.vtkPoints()
        for xyzPoint in self.InputDict['Points']:
            points.InsertNextPoint(xyzPoint)

        polyData.SetPoints(points)

        pointDataKeys = self.InputDict['PointData'].keys()
        for key in pointDataKeys:

            pointDataDType = str(self.InputDict['PointData'][key].dtype)
            if pointDataDType.find('float') != -1:
                pointDataArray = vtk.vtkFloatArray()
            if pointDataDType.find('int') != -1:
                pointDataArray = vtk.vtkIntArray()

            pointDataArray.SetNumberOfComponents(1)
            pointDataArray.SetName(key)

            for pointData in self.InputDict['PointData'][key]:
                pointDataArray.InsertNextValue(pointData)

            polyData.GetPointData().SetActiveScalars(key)
            polyData.GetPointData().SetScalars(pointDataArray)

        def mkVtkIdList(it):
            vil = vtk.vtkIdList()
            for i in it:
                vil.InsertNextId(int(i))
            return vil

        cellDataArray = vtk.vtkCellArray()
        for cell in self.InputDict['CellData']['CellPointIds']:
            cellDataArray.InsertNextCell(mkVtkIdList(cell))

        polyData.SetPolys(cellDataArray)

        self.Surface = polyData

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()