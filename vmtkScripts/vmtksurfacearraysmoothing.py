#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacearraysmoothing.py,v $
## Language:  Python
## Date:      $Date: 2014/10/24 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

import vtk
import sys
from vmtk import pypes
import math

vmtksurfacearraysmoothing = 'vmtkSurfaceArraySmoothing'

class vmtkSurfaceArraySmoothing(pypes.pypeScript):


    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.SurfaceArrayName = 'SurfaceArray'
        self.Connexity = 1

        self.SetScriptName('vmtksurfacearraysmoothing')
        self.SetScriptDoc('')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['SurfaceArrayName','surfacearray','str',1],
            ['Connexity','connexity','int',1,'(1,2)','patch connexity considered in the smoothing procedure'],
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
        extractEdges.SetInput(surface)
        extractEdges.Update()
        surfEdges = extractEdges.GetOutput()

        if self.Connexity == 1:
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
                vval = vval + val
                newval = vval / (N+1)
                array.SetTuple1(i,newval)
        elif self.Connexity == 2:
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
                vval = vval + val
                newval = vval / (N+1)
                array.SetTuple1(i,newval)
        else:
            self.PrintError ('Error: wrong connexity')

        self.Surface = surface

if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
