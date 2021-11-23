## Program:   VMTK
## Module:    $RCSfile: vmtksurfacearraysmoothing.py,v $
## Language:  Python
## Date:      $Date: 2014/10/24 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
from vmtk import pypes
import math


class vmtkSurfaceArraySmoothing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.SurfaceArrayName = ''
        self.Connexity = 1
        self.Relaxation = 1.0
        self.Iterations = 1

        self.SetScriptName('vmtksurfacearraysmoothing')
        self.SetScriptDoc('Perform smoothing of the point array defined on the surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['SurfaceArrayName','surfacearray','str',1],
            ['Connexity','connexity','int',1,'(1,2)','patch connexity considered in the smoothing procedure'],
            ['Relaxation','relaxation','float',1,'(0.0,1.0)','relaxation factor'],
            ['Iterations','iterations','int',1,'','number of smoothing iterations']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface with the smoothed array','vmtksurfacewriter'],
            ])

    def Execute(self):

        if (self.Surface == None):
            self.PrintError('Error: no Surface.')

        if (self.Surface.GetPointData().GetArray(self.SurfaceArrayName) == None):
            self.PrintError('Error: no array with name specified')
        else:
            array = self.Surface.GetPointData().GetArray(self.SurfaceArrayName)

        surface = self.Surface
        extractEdges = vtk.vtkExtractEdges()
        extractEdges.SetInputData(surface)
        extractEdges.Update()
        surfEdges = extractEdges.GetOutput()

        if self.Connexity == 1:
            for n in range (self.Iterations):
                for i in range (surfEdges.GetNumberOfPoints()):
                    cells = vtk.vtkIdList()
                    surfEdges.GetPointCells(i,cells)
                    vval = 0
                    ddd = 0
                    d = 0
                    N = 0
                    for j in range (cells.GetNumberOfIds()):
                        points = vtk.vtkIdList()
                        surfEdges.GetCellPoints(cells.GetId(j),points)
                        for k in range (points.GetNumberOfIds()):
                            if points.GetId(k) != i:
                                d = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(surface.GetPoint(i),surface.GetPoint(points.GetId(k))))
                                dd = 1/d
                                val = array.GetComponent(points.GetId(k),0)#*dd
                                N = N+1
                                vval = vval + val
                                ddd = ddd + dd
                    val = array.GetComponent(i,0)
                    vval = vval / (N)
                    newval = self.Relaxation * vval + (1 - self.Relaxation) * val
                    array.SetTuple1(i,newval)
        elif self.Connexity == 2:
            for n in range (self.Iterations):
                for i in range (surfEdges.GetNumberOfPoints()):
                    cells = vtk.vtkIdList()
                    surfEdges.GetPointCells(i,cells)
                    pointlist = vtk.vtkIdList()
                    vval = 0
                    ddd = 0
                    d = 0
                    N = 0
                    for j in range (cells.GetNumberOfIds()):
                        points = vtk.vtkIdList()
                        surfEdges.GetCellPoints(cells.GetId(j),points)
                        for k in range (points.GetNumberOfIds()):
                            if points.GetId(k) != i:
                                pointlist.InsertUniqueId(points.GetId(k))
                                cells2 = vtk.vtkIdList()
                                surfEdges.GetPointCells(i,cells2)
                                for p in range (cells2.GetNumberOfIds()):
                                    points2 = vtk.vtkIdList()
                                    surfEdges.GetCellPoints(cells2.GetId(j),points2)
                                    for q in range (points2.GetNumberOfIds()):
                                        if points2.GetId(k) != i:
                                            pointlist.InsertUniqueId(points2.GetId(k))
                    N = pointlist.GetNumberOfIds()
                    for j in range (pointlist.GetNumberOfIds()):
                        d = math.sqrt(vtk.vtkMath.Distance2BetweenPoints(surface.GetPoint(i),surface.GetPoint(pointlist.GetId(j))))
                        dd = 1/d
                        val = array.GetComponent(pointlist.GetId(j),0)
                        vval = vval + val
                        ddd = ddd + dd
                    val = array.GetComponent(i,0)
                    vval = vval / (N)
                    newval = self.Relaxation * vval + (1 - self.Relaxation) * val
                    array.SetTuple1(i,newval)
        else:
            self.PrintError ('Error: wrong connexity')

        self.Surface = surface


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
