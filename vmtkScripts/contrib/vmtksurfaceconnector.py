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
from vmtk import vtkvmtk
from vmtk import pypes


class vmtkSurfaceConnector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)


        self.Surface = None
        self.Surface2 = None
        self.Ring = None
        self.Ring2 = None
        self.OutputSurface = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.IdValue = 1
        self.CleanOutput = 1

        self.Display = 0
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Actor = None
        self.Representation = 'edges'

        self.SurfaceIsRendered = False
        self.Surface2IsRendered = False

        self.SetScriptName('vmtksurfaceconnector')
        self.SetScriptDoc('connect two rings of two different surfaces; if a surface has more than a boundary ring, an interactive interface allows to select the ring to connect')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the first input surface','vmtksurfacereader'],
            ['Surface2','i2','vtkPolyData',1,'','the second input surface','vmtksurfacereader'],
            ['Ring','iring','vtkPolyData',1,'','the first input ring','vmtksurfacereader'],
            ['Ring2','i2ring','vtkPolyData',1,'','the second input ring','vmtksurfacereader'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,'',''],
            ['IdValue','idvalue','int',1,'','entity id value in the connecting surface'],
            ['CleanOutput','cleanoutput','bool',1,'','clean the unused points in the output and triangulate it'],
            ['Display','display','bool',1,'','toggle rendering while algorithm advances'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['OutputSurface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
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


    def ViewSurface(self,polydata,color,opacity=1.0,representation=None):
        if representation==None:
            representation = self.Representation

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(polydata)
        mapper.ScalarVisibilityOff()
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.Actor.GetProperty().SetColor(color)
        self.Actor.GetProperty().SetOpacity(opacity)

        self.SetSurfaceRepresentation(representation)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)
        self.vmtkRenderer.AddKeyBinding('w','Change surface representation.',self.RepresentationCallback)
        self.vmtkRenderer.Render()


    def ViewTriangle(self,triangle,color):
        trianglePolydata = vtk.vtkPolyData()
        triangleCell = vtk.vtkCellArray()
        triangleCell.InsertNextCell(triangle)
        trianglePolydata.SetPoints(self.OutputSurface.GetPoints())
        trianglePolydata.SetPolys(triangleCell)
        self.ViewSurface(trianglePolydata,color)


    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1


    def InteractiveRingExtraction(self,surface):

        boundaryIds = vtk.vtkIdList()

        featureEdges = vtk.vtkFeatureEdges()
        featureEdges.BoundaryEdgesOn()
        featureEdges.FeatureEdgesOff()
        featureEdges.NonManifoldEdgesOff()
        featureEdges.ManifoldEdgesOff()
        featureEdges.ColoringOff()
        featureEdges.SetInputData(surface)
        featureEdges.CreateDefaultLocator()
        featureEdges.Update()
        rings = featureEdges.GetOutput()

        connectivity = vtk.vtkConnectivityFilter()
        connectivity.SetInputData(rings)
        connectivity.SetExtractionModeToAllRegions()
        connectivity.ColorRegionsOn()
        connectivity.Update()
        rings = connectivity.GetOutput()

        ringIdsArray = rings.GetCellData().GetArray('RegionId')

        ringIds = set()
        for i in range(rings.GetNumberOfCells()):
            ringIds.add(ringIdsArray.GetComponent(i,0))
        ringIds = sorted(ringIds)
        numberOfRings = len(ringIds)

        boundaries = []
        for i in range(numberOfRings):
            th = vtk.vtkThreshold()
            th.SetInputData(rings)
            th.ThresholdBetween(ringIds[i]-0.5,ringIds[i]+0.5)
            th.Update()
            gf = vtk.vtkGeometryFilter()
            gf.SetInputConnection(th.GetOutputPort())
            gf.Update()
            boundaries.append(gf.GetOutput())

        numberOfBoundaries = numberOfRings

        if numberOfBoundaries>1:

            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            seedPoints = vtk.vtkPoints()
            for i in range(numberOfRings):
                barycenter = [0.0, 0.0, 0.0]
                vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(boundaries[i].GetPoints(),barycenter)
                seedPoints.InsertNextPoint(barycenter)
            seedPolyData = vtk.vtkPolyData()
            seedPolyData.SetPoints(seedPoints)
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInputData(seedPolyData)
            labelsMapper.SetLabelModeToLabelIds()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)


            self.vmtkRenderer.Renderer.AddActor(labelsActor)

            surfaceMapper = vtk.vtkPolyDataMapper()
            surfaceMapper.SetInputData(surface)
            surfaceMapper.ScalarVisibilityOff()
            surfaceActor = vtk.vtkActor()
            surfaceActor.SetMapper(surfaceMapper)
            surfaceActor.GetProperty().SetOpacity(0.25)
            #surfaceActor.GetProperty().SetColor(color)

            self.vmtkRenderer.Renderer.AddActor(surfaceActor)

            ok = False
            if self.Surface2 == None:
                maxNumberOfBoundaries = 2
            else:
                maxNumberOfBoundaries = 1
            while not ok :
                labelString = self.InputText("Please input "+str(maxNumberOfBoundaries)+" boundary ids: ",self.LabelValidator)
                labels = [int(label) for label in labelString.split()]
                print(labels,len(labels))
                ok = True
                for label in labels:
                    if label not in list(range(numberOfBoundaries)):
                        ok = False
                if len(labels) != maxNumberOfBoundaries:
                    ok = False

            for label in labels:
                boundaryIds.InsertNextId(label)

            #self.vmtkRenderer.Render()
            surfaceActor.GetProperty().SetOpacity(0.1)
            surfaceActor.GetProperty().SetColor([1.,1.,1.])
            self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
            #self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)

            return boundaries, boundaryIds, True
        
        boundaryIds.InsertNextId(0)
        return boundaries, boundaryIds, False


    def InitializeRingsFromSurfaces(self):
        [boundaries,boundaryIds,self.SurfaceIsRendered] = self.InteractiveRingExtraction(self.Surface)

        numIds = boundaryIds.GetNumberOfIds()

        self.Ring = boundaries[boundaryIds.GetId(0)]
        if numIds > 1:
            self.Ring2 = boundaries[boundaryIds.GetId(1)]
        else:
            [boundaries2,boundaryId2,self.Surface2IsRendered] = self.InteractiveRingExtraction(self.Surface2)
            self.Ring2 = boundaries2[boundaryId2.GetId(0)]

    def MergeSurfacesAndEntityIdsArray(self):

        def addEntityIdsArray(surface,idValue):
            cellEntityIdsArray = surface.GetCellData().GetArray(self.CellEntityIdsArrayName)
            if cellEntityIdsArray == None:
                cellEntityIdsArray = vtk.vtkIntArray()
                cellEntityIdsArray.SetName(self.CellEntityIdsArrayName)
                cellEntityIdsArray.SetNumberOfComponents(1)
                cellEntityIdsArray.SetNumberOfTuples(surface.GetNumberOfCells())
                surface.GetCellData().AddArray(cellEntityIdsArray)
                cellEntityIdsArray.FillComponent(0,idValue)

        addEntityIdsArray(self.OutputSurface,self.IdValue)
        if self.Surface:
            addEntityIdsArray(self.Surface,self.IdValue-1)
        if self.Surface2:
            addEntityIdsArray(self.Surface2,self.IdValue+1)

        mergeSurfaces = vtk.vtkAppendPolyData()
        mergeSurfaces.AddInputData(self.OutputSurface)
        if self.Surface:
            mergeSurfaces.AddInputData(self.Surface)
        if self.Surface2:
            mergeSurfaces.AddInputData(self.Surface2)
        mergeSurfaces.Update()
        self.OutputSurface = mergeSurfaces.GetOutput()

        if self.CleanOutput:
            from vmtk import vmtkscripts
            cleaner = vmtkscripts.vmtkSurfaceTriangle()
            cleaner.Surface = self.OutputSurface
            cleaner.Execute()
            self.OutputSurface = cleaner.Surface



    def Execute(self):

        if self.Surface:
            self.InitializeRingsFromSurfaces()

        if self.Ring == None and self.Ring2 == None:
            self.PrintError('Error: No input surface or rings.')

        red = [1.,0.,0.]
        green = [0.,1.,0.]
        blue = [0.,0.,1.]
        white = [1.,1.,1.]

        n1 = self.Ring.GetNumberOfPoints()
        n2 = self.Ring2.GetNumberOfPoints()

        points = vtk.vtkPoints()
        cells = vtk.vtkCellArray()

        self.OutputSurface = vtk.vtkPolyData()
        self.OutputSurface.SetPoints(points)
        self.OutputSurface.SetPolys(cells)

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

        # initial rendering
        if self.Display:
            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1
            self.vmtkRenderer.RegisterScript(self) 
            if self.Actor:
                self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

            if self.Surface and not self.SurfaceIsRendered:
                self.ViewSurface(self.Surface,white,0.1,'surface')
                self.SurfaceIsRendered = True
            if self.Surface2 and not self.Surface2IsRendered:
                self.ViewSurface(self.Surface2,white,0.1,'surface')
                self.Surface2IsRendered = True

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

        self.OutputSurface.BuildLinks()
        self.MergeSurfacesAndEntityIdsArray()




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
