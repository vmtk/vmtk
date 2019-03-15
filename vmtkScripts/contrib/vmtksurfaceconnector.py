#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceappend.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys
import numpy as np

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkSurfaceConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)


        self.Surface = None
        self.Surface2 = None
        self.Ring = None
        self.Ring2 = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        #self.Method = 'simple'

        self.Display = 0
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Actor = None
        self.Representation = 'edges'

        self.SetScriptName('vmtksurfaceconnector')
        self.SetScriptDoc('connect two rings of two different surfaces.')
        self.SetInputMembers([
            ['Surface','isurface','vtkPolyData',1,'','the first input surface','vmtksurfacereader'],
            ['Surface2','i2surface','vtkPolyData',1,'','the second input surface','vmtksurfacereader'],
            ['Ring','i','vtkPolyData',1,'','the first input ring','vmtksurfacereader'],
            ['Ring2','i2','vtkPolyData',1,'','the second input ring','vmtksurfacereader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'',''],
            #['Method','method','str',1,'["simple","delaunay"]','connecting method']
            ['Display','display','bool',1,'','toggle rendering while algorithm advances'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['Actor','oactor','vtkActor',1,'','the output actor']
            ])


    def SetSurfaceRepresentation(self, representation):
        if representation == 'surface':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOff()
        elif representation == 'edges':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOn()
        elif representation == 'wireframe':
            self.Actor.GetProperty().SetRepresentationToWireframe()
            self.Actor.GetProperty().EdgeVisibilityOff()
        self.Representation = representation


    def RepresentationCallback(self, obj):
        if not self.Actor:
            return
        if self.Representation == 'surface':
            representation = 'edges'
        elif self.Representation == 'edges':
            representation = 'wireframe'
        elif self.Representation == 'wireframe':
            representation = 'surface'
        self.SetSurfaceRepresentation(representation)
        self.vmtkRenderer.RenderWindow.Render()


    def ViewSurface(self,polydata,color):
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(polydata)
        mapper.ScalarVisibilityOff()
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.Actor.GetProperty().SetColor(color)
        self.SetSurfaceRepresentation(self.Representation)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)
        self.vmtkRenderer.AddKeyBinding('w','Change surface representation.',self.RepresentationCallback)
        self.vmtkRenderer.Render()


    def ViewTriangle(self,triangle,color):
            trianglePolydata = vtk.vtkPolyData()
            triangleCell = vtk.vtkCellArray()
            triangleCell.InsertNextCell(triangle)
            trianglePolydata.SetPoints(self.Surface.GetPoints())
            trianglePolydata.SetPolys(triangleCell)
            self.ViewSurface(trianglePolydata,color)


    def Execute(self):

        if self.Surface and self.Surface2:
            self.PrintError('Error: Option with input surfaces not yet implemented.')

        if self.Ring == None and self.Ring2 == None:
            self.PrintError('Error: No input rings.')

        n1 = self.Ring.GetNumberOfPoints()
        n2 = self.Ring2.GetNumberOfPoints()

        points = vtk.vtkPoints()
        cells = vtk.vtkCellArray()

        self.Surface = vtk.vtkPolyData()
        self.Surface.SetPoints(points)
        self.Surface.SetPolys(cells)

        points1 = self.Ring.GetPoints()
        points2 = self.Ring2.GetPoints()
        points.SetNumberOfPoints(n1+n2)
        for i in range(n1):
            points.SetPoint(i, self.Ring.GetPoint(i))
        for i in range(n2):
            points.SetPoint(i+n1, self.Ring2.GetPoint(i))

        print("Ring1: ",n1," points")
        print("Ring2: ",n2," points")
        print("Total: ",n1+n2," points")

        red = [1.,0.,0.]
        green = [0.,1.,0.]
        blue = [0.,0.,1.]
        white = [1.,1.,1.]

        # initial rendering
        if self.Display:
            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1
            self.vmtkRenderer.RegisterScript(self) 
            if self.Actor:
                self.vmtkRenderer.Renderer.RemoveActor(self.Actor)
            self.ViewSurface(self.Ring,red)
            self.ViewSurface(self.Ring2,green)
 
        def nextPointId(ring,cellId,currentPointId):
            idList = vtk.vtkIdList()
            ring.GetCellPoints(cellId,idList)
            if idList.GetId(0) == currentPointId:
                return idList.GetId(1)
            else:
                return idList.GetId(0)

        def nextCellId(ring,pointId,currentCellId):
            idList = vtk.vtkIdList()
            ring.GetPointCells(pointId,idList)
            if idList.GetId(0) == currentCellId:
                return idList.GetId(1)
            else:
                return idList.GetId(0)

        def insertNextTriangle(cells,idA,idB,idC,color):
            cell = vtk.vtkTriangle()
            cell.GetPointIds().SetId(0,idA)
            cell.GetPointIds().SetId(1,idB)
            cell.GetPointIds().SetId(2,idC)
            cells.InsertNextCell(cell)
            if self.Display:
                self.ViewTriangle(cell,color)

        # initialize cellId1, cellId2, pointId1, pointId2
        cellId1 = 0 #self.Ring.GetCells.GetId(0)
        pointIdList = vtk.vtkIdList()
        cellIdList = vtk.vtkIdList()

        self.Ring.GetCellPoints(cellId1,pointIdList)
        pointId1 = pointIdList.GetId(0)
        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self.Ring2)
        pointLocator.BuildLocator()
        pointId2 = pointLocator.FindClosestPoint(self.Ring.GetPoint(pointId1))
        self.Ring2.GetPointCells(pointId2,cellIdList)
        testPointIdA = nextPointId(self.Ring2,cellIdList.GetId(0),pointId2)
        testPointIdB = nextPointId(self.Ring2,cellIdList.GetId(1),pointId2)

        math = vtk.vtkMath()
        dA = math.Distance2BetweenPoints(self.Ring.GetPoint(pointIdList.GetId(1)),self.Ring2.GetPoint(testPointIdA))
        dB = math.Distance2BetweenPoints(self.Ring.GetPoint(pointIdList.GetId(1)),self.Ring2.GetPoint(testPointIdB))

        if dA < dB:
            cellId2 = cellIdList.GetId(0)
        else:
            cellId2 = cellIdList.GetId(1)

        firstPointId1 = pointId1
        firstPointId2 = pointId2

        iteration = 1

        while iteration < (n1+n2):
            d1 = math.Distance2BetweenPoints( self.Ring.GetPoint(pointId1), self.Ring2.GetPoint(nextPointId(self.Ring2,cellId2,pointId2)) )
            d2 = math.Distance2BetweenPoints( self.Ring2.GetPoint(pointId2), self.Ring.GetPoint(nextPointId(self.Ring,cellId1,pointId1)) )
            if d1>d2:
                insertNextTriangle( cells, pointId1, pointId2+n1, nextPointId(self.Ring,cellId1,pointId1), red )
                pointId1 = nextPointId(self.Ring,cellId1,pointId1)
                cellId1 = nextCellId(self.Ring,pointId1,cellId1)
            else:
                insertNextTriangle( cells, pointId2+n1, nextPointId(self.Ring2,cellId2,pointId2)+n1, pointId1, green )
                pointId2 = nextPointId(self.Ring2,cellId2,pointId2)
                cellId2 = nextCellId(self.Ring2,pointId2,cellId2)
            iteration = iteration+1

        # last triangle insertion
        if pointId1 == firstPointId1:
            insertNextTriangle( cells, pointId2+n1, nextPointId(self.Ring2,cellId2,pointId2)+n1, pointId1, blue )
        else:
            insertNextTriangle( cells, pointId1, pointId2+n1, nextPointId(self.Ring,cellId1,pointId1), blue )

        self.Surface = vtk.vtkPolyData()
        self.Surface.SetPoints(points)
        self.Surface.SetPolys(cells)
        self.Surface.BuildLinks()
 


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
