#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlines.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:52:56 $
## Version:   $Revision: 1.20 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


## TODO: make SeedSelector a separate pype script to be used in other contexts
class vmtkSeedSelector(object):

    def __init__(self):
        self._Surface = None
        self._SeedIds = None
        self._SourceSeedIds = vtk.vtkIdList()
        self._TargetSeedIds = vtk.vtkIdList()
        self.PrintError = None
        self.PrintLog = None
        self.InputText = None
        self.OutputText = None
        self.InputInfo = None

    def SetSurface(self,surface):
        self._Surface = surface

    def GetSurface(self):
        return self._Surface

    def GetSourceSeedIds(self):
        return self._SourceSeedIds

    def GetTargetSeedIds(self):
        return self._TargetSeedIds

    def Execute(self):
        pass


class vmtkIdListSeedSelector(vmtkSeedSelector):

    def __init__(self):
        vmtkSeedSelector.__init__(self)
        self.SourceIds = None
        self.TargetIds = None

    def Execute(self):

        if not self._Surface:
            self.PrintError('vmtkIdListSeedSelector Error: Surface not set.')
            return

        if not self.SourceIds:
            self.PrintError('vmtkIdListSeedSelector Error: SourceIds not set.')
            return

        if not self.TargetIds:
            self.PrintError('vmtkIdListSeedSelector Error: TargetIds not set.')
            return

        self._SourceSeedIds.Initialize()
        self._TargetSeedIds.Initialize()

        maxId = self._Surface.GetNumberOfPoints()-1

        for id in self.SourceIds:
            if id > maxId:
              self.PrintError('vmtkIdListSeedSelector Error: invalid SourceId.')
            self._SourceSeedIds.InsertNextId(id)

        for id in self.TargetIds:
            if id > maxId:
              self.PrintError('vmtkIdListSeedSelector Error: invalid TargetId.')
            self._TargetSeedIds.InsertNextId(id)


class vmtkPointListSeedSelector(vmtkSeedSelector):

    def __init__(self):
        vmtkSeedSelector.__init__(self)
        self.SourcePoints = None
        self.TargetPoints = None

    def Execute(self):

        if not self._Surface:
            self.PrintError('vmtkPointListSeedSelector Error: Surface not set.')
            return

        if not self.SourcePoints:
            self.PrintError('vmtkPointListSeedSelector Error: SourcePoints not set.')
            return

        if not self.TargetPoints:
            self.PrintError('vmtkPointListSeedSelector Error: TargetPoints not set.')
            return

        self._SourceSeedIds.Initialize()
        self._TargetSeedIds.Initialize()

        if len(self.SourcePoints) % 3 != 0:
            self.PrintError('vmtkPointListSeedSelector Error: SourcePoints not made up of triplets.')
            return

        if len(self.TargetPoints) % 3 != 0:
            self.PrintError('vmtkPointListSeedSelector Error: TargetPoints not made up of triplets.')
            return

        pointLocator = vtk.vtkPointLocator()
        pointLocator.SetDataSet(self._Surface)
        pointLocator.BuildLocator()

        for i in range(len(self.SourcePoints)//3):
            point = [self.SourcePoints[3*i+0],self.SourcePoints[3*i+1],self.SourcePoints[3*i+2]]
            id = pointLocator.FindClosestPoint(point)
            self._SourceSeedIds.InsertNextId(id)

        for i in range(len(self.TargetPoints)//3):
            point = [self.TargetPoints[3*i+0],self.TargetPoints[3*i+1],self.TargetPoints[3*i+2]]
            id = pointLocator.FindClosestPoint(point)
            self._TargetSeedIds.InsertNextId(id)


class vmtkPickPointSeedSelector(vmtkSeedSelector):

    def __init__(self):
        vmtkSeedSelector.__init__(self)
        self.PickedSeedIds = vtk.vtkIdList()
        self.PickedSeeds = vtk.vtkPolyData()
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Script = None

    def UndoCallback(self, obj):
        self.InitializeSeeds()
        self.PickedSeeds.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def PickCallback(self, obj):
        picker = vtk.vtkCellPicker()
        picker.SetTolerance(1E-4 * self._Surface.GetLength())
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        if result == 0:
            return
        pickPosition = picker.GetPickPosition()
        pickedCellPointIds = self._Surface.GetCell(picker.GetCellId()).GetPointIds()
        minDistance = 1E10
        pickedSeedId = -1
        for i in range(pickedCellPointIds.GetNumberOfIds()):
            distance = vtk.vtkMath.Distance2BetweenPoints(pickPosition,self._Surface.GetPoint(pickedCellPointIds.GetId(i)))
            if distance < minDistance:
                minDistance = distance
                pickedSeedId = pickedCellPointIds.GetId(i)
        if pickedSeedId == -1:
            pickedSeedId = pickedCellPointIds.GetId(0)
        self.PickedSeedIds.InsertNextId(pickedSeedId)
        point = self._Surface.GetPoint(pickedSeedId)
        self.PickedSeeds.GetPoints().InsertNextPoint(point)
        self.PickedSeeds.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def InitializeSeeds(self):
        self.PickedSeedIds.Initialize()
        self.PickedSeeds.Initialize()
        seedPoints = vtk.vtkPoints()
        self.PickedSeeds.SetPoints(seedPoints)

    def Execute(self):

        if (self._Surface == None):
            self.PrintError('vmtkPickPointSeedSelector Error: Surface not set.')
            return

        self._SourceSeedIds.Initialize()
        self._TargetSeedIds.Initialize()

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self.Script)

        glyphs = vtk.vtkGlyph3D()
        glyphSource = vtk.vtkSphereSource()
        glyphs.SetInputData(self.PickedSeeds)
        glyphs.SetSourceConnection(glyphSource.GetOutputPort())
        glyphs.SetScaleModeToDataScalingOff()
        glyphs.SetScaleFactor(self._Surface.GetLength()*0.01)
        glyphMapper = vtk.vtkPolyDataMapper()
        glyphMapper.SetInputConnection(glyphs.GetOutputPort())
        self.SeedActor = vtk.vtkActor()
        self.SeedActor.SetMapper(glyphMapper)
        self.SeedActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.SeedActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.SeedActor)

        ##self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)
        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('space','Add points.',self.PickCallback)
        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputData(self._Surface)
        surfaceMapper.ScalarVisibilityOff()
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(surfaceMapper)
        surfaceActor.GetProperty().SetOpacity(1.0)

        self.vmtkRenderer.Renderer.AddActor(surfaceActor)

        self.InputInfo('Please position the mouse and press space to add source points, \'u\' to undo\n')

        any = 0
        while any == 0:
            self.InitializeSeeds()
            self.vmtkRenderer.Render()
            any = self.PickedSeedIds.GetNumberOfIds()
        self._SourceSeedIds.DeepCopy(self.PickedSeedIds)

        self.InputInfo('Please position the mouse and press space to add target points, \'u\' to undo\n')

        any = 0
        while any == 0:
            self.InitializeSeeds()
            self.vmtkRenderer.Render()
            any = self.PickedSeedIds.GetNumberOfIds()
        self._TargetSeedIds.DeepCopy(self.PickedSeedIds)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


class vmtkOpenProfilesSeedSelector(vmtkSeedSelector):

    def __init__(self):
        vmtkSeedSelector.__init__(self)
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Script = None

    def SetSeedIds(self,seedIds):
        self._SeedIds = seedIds

    def GetSeedIds(self):
        return self._SeedIds

    def Execute(self):

        if (self._Surface == None):
            self.PrintError('vmtkOpenProfilesSeedSelector Error: Surface not set.')
            return

        if (self._SeedIds == None):
            self.PrintError('vmtkOpenProfilesSeedSelector Error: SeedIds not set.')
            return

        self._SourceSeedIds.Initialize()
        self._TargetSeedIds.Initialize()

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self.Script)

        seedPoints = vtk.vtkPoints()
        for i in range(self._SeedIds.GetNumberOfIds()):
            seedPoints.InsertNextPoint(self._Surface.GetPoint(self._SeedIds.GetId(i)))
        seedPolyData = vtk.vtkPolyData()
        seedPolyData.SetPoints(seedPoints)
        labelsMapper = vtk.vtkLabeledDataMapper();
        labelsMapper.SetInputData(seedPolyData)
        labelsMapper.SetLabelModeToLabelIds()
        labelsActor = vtk.vtkActor2D()
        labelsActor.SetMapper(labelsMapper)

        self.vmtkRenderer.Renderer.AddActor(labelsActor)

        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputData(self._Surface)
        surfaceMapper.ScalarVisibilityOff()
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(surfaceMapper)
        surfaceActor.GetProperty().SetOpacity(0.25)

        self.vmtkRenderer.Renderer.AddActor(surfaceActor)

        self.vmtkRenderer.Render()

        seedIdString = self.InputText("Please input list of inlet profile ids: ")
        separator = ' '
        if seedIdString.find(',') != -1:
            separator = ','
        seedIdList = seedIdString.split(separator)
        for seedIdString in seedIdList:
            self._SourceSeedIds.InsertNextId(int(seedIdString.strip()))

        seedIdString = self.InputText("Please input list of outlet profile ids (leave empty for all available profiles): ")
        if seedIdString.strip() == '':
            self.InputInfo("Selected outlets: ")
            for i in range(seedPoints.GetNumberOfPoints()):
                if self._SourceSeedIds.IsId(i) == -1:
                    self._TargetSeedIds.InsertNextId(i)
                    self.InputInfo("%d " % i)
            self.InputInfo("\n")
        else:
            seedIdList = seedIdString.split(separator)
            for seedIdString in seedIdList:
                self._TargetSeedIds.InsertNextId(int(seedIdString.strip()))

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


class vmtkCarotidProfilesSeedSelector(vmtkSeedSelector):

    def SetSeedIds(self,seedIds):
        self._SeedIds = seedIds

    def GetSeedIds(self):
        return self._SeedIds

    def Execute(self):

        if (self._Surface == None):
            self.PrintError('vmtkCarotidProfilesSeedSelector Error: Surface not set.')
            return

        if (self._SeedIds == None):
            self.PrintError('vmtkCarotidProfilesSeedSelector Error: SeedIds not set.')
            return

        self._SourceSeedIds.Initialize()
        self._TargetSeedIds.Initialize()

        minZId = -1
        maxZId = -1
        minZ = 1E8
        maxZ = -1E8

        for id in range(self._SeedIds.GetNumberOfIds()):
            currentZ = self._Surface.GetPoint(self._SeedIds.GetId(id))[2]
            if (currentZ < minZ):
                minZ = currentZ
                minZId = id

            if (currentZ > maxZ):
                maxZ = currentZ
                maxZId = id

        self._SourceSeedIds.InsertNextId(minZId)
        self._TargetSeedIds.InsertNextId(maxZId)

        midZId = 3-(minZId+maxZId)
        self._TargetSeedIds.InsertNextId(midZId)


class vmtkNonManifoldSurfaceChecker(object):

    def __init__(self):

        self.Surface = 0

        self.NumberOfNonManifoldEdges = 0
        self.Report = 0
        self.NonManifoldEdgePointIds = vtk.vtkIdList()

        self.PrintError = None

    def Execute(self):

        if (self.Surface == 0):
            self.PrintError('NonManifoldSurfaceChecker error: Surface not set')
            return

        self.NonManifoldEdgesFound = 0
        self.Report = ''
        self.NonManifoldEdgePointIds.Initialize()

        neighborhoods = vtkvmtk.vtkvmtkNeighborhoods()
        neighborhoods.SetNeighborhoodTypeToPolyDataManifoldNeighborhood()
        neighborhoods.SetDataSet(self.Surface)
        neighborhoods.Build()

        neighborCellIds = vtk.vtkIdList()
        cellPointIds = vtk.vtkIdList()

        self.Surface.BuildCells()
        self.Surface.BuildLinks(0)

        numberOfNonManifoldEdges = 0

        for i in range(neighborhoods.GetNumberOfNeighborhoods()):

            neighborhood = neighborhoods.GetNeighborhood(i)

            for j in range(neighborhood.GetNumberOfPoints()):

                neighborId = neighborhood.GetPointId(j)

                if (i<neighborId):

                    neighborCellIds.Initialize()
                    self.Surface.GetCellEdgeNeighbors(-1,i,neighborId,neighborCellIds)

                    if (neighborCellIds.GetNumberOfIds()>2):

                        numberOfNonManifoldEdges = numberOfNonManifoldEdges + 1

                        self.Report = self.Report +  "Non-manifold edge found" + str(i) + ' ' + str(neighborId) + '.\n'

                        self.NonManifoldEdgePointIds.InsertNextId(i)
                        self.NonManifoldEdgePointIds.InsertNextId(neighborId)


class vmtkCenterlines(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.SeedSelector = None
        self.SeedSelectorName = 'pickpoint'
        self.FlipNormals = 0
        self.CapDisplacement = 0.0
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.CostFunction = '1/R'
        self.AppendEndPoints = 0
        self.CheckNonManifold = 0

        self.Resampling = 0
        self.ResamplingStepLength = 1.0
        self.SimplifyVoronoi = 0

        self.EikonalSolutionArrayName = 'EikonalSolution'
        self.EdgeArrayName = 'EdgeArray'
        self.EdgePCoordArrayName = 'EdgePCoordArray'
        self.CostFunctionArrayName = 'CostFunctionArray'

        self.UseTetGen = 0
        self.TetGenDetectInter = 1

        self.DelaunayTessellation = None
        self.VoronoiDiagram = None
        self.PoleIds = None

        self.DelaunayTolerance = 0.001

        self.SourceIds = []
        self.TargetIds = []
        self.SourcePoints = []
        self.TargetPoints = []

        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.StopFastMarchingOnReachingTarget = 0

        self.SetScriptName('vmtkcenterlines')
        self.SetScriptDoc('compute centerlines from a branching tubular surface (see papers for details); seed points can be interactively selected on the surface, or specified as the barycenters of the open boundaries of the surface; if vmtk is compiled with support for TetGen, TetGen can be employed to compute the Delaunay tessellation of the input points')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['SeedSelectorName','seedselector','str',1,'["pickpoint","openprofiles","carotidprofiles","profileidlist","idlist","pointlist"]','seed point selection method (pickpoint: interactive; openprofiles: choose among barycenters of open profiles of the surface; carotidprofiles: open profiles are automatically selected based on their z-axis coordinate (lower to higher: CCA, ECA, ICA)); profileidlist: list of open profile ids (specified as argument to -sourceids and -targetids); idlist: list of surface point ids (specified as argument to -sourceids and -targetids); pointlist: list of surface points (specified as argument to -sourcepoints and -targetpoints)'],
            ['SourceIds','sourceids','int',-1,'','list of source point ids'],
            ['TargetIds','targetids','int',-1,'','list of target point ids'],
            ['SourcePoints','sourcepoints','float',-1,'','list of source point coordinates'],
            ['TargetPoints','targetpoints','float',-1,'','list of target point coordinates'],
            ['AppendEndPoints','endpoints','bool',1,'','toggle append open profile barycenters to centerlines'],
            ['CheckNonManifold','nonmanifoldcheck','bool',1,'','toggle checking the surface for non-manifold edges'],
            ['FlipNormals','flipnormals','bool',1,'','flip normals after outward normal computation; outward oriented normals must be computed for the removal of outer tetrahedra; the algorithm might fail so for weird geometries, so changing this might solve the problem'],
            ['CapDisplacement','capdisplacement','float',1,'','displacement of the center points of caps at open profiles along their normals (avoids the creation of degenerate tetrahedra)'],
            ['DelaunayTolerance','delaunaytolerance','float',1,'','tolerance for evaluating coincident points during Delaunay tessellation, evaluated as a fraction of the bounding box'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres have to be stored'],
            ['AppendEndPoints','endpoints','bool',1,'','toggle append open profile barycenters to centerlines'],
            ['Resampling','resampling','bool',1,'','toggle centerlines resampling'],
            ['ResamplingStepLength','resamplingstep','float',1,'(0.0,)','distance between points in the resampled line'],
            ['DelaunayTessellation','delaunaytessellation','vtkUnstructuredGrid',1,'','optional input Delaunay tessellation'],
            ['SimplifyVoronoi','simplifyvoronoi','bool',1,'','toggle simplification of Voronoi diagram'],
            ['UseTetGen','usetetgen','bool',1,'','toggle use TetGen to compute Delaunay tessellation'],
            ['TetGenDetectInter','tetgendetectinter','bool',1,'','TetGen option'],
            ['CostFunction','costfunction','str',1,'','specify cost function to be minimized during centerline computation'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['PoleIds','poleids','vtkIdList',1],
            ['VoronoiDiagram','voronoidiagram','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['StopFastMarchingOnReachingTarget','stopontarget','bool',1,'','terminate fast marching front propagation when the front reaches the target seed point. This can greatly speed up execution, but it only works when one target seed id is set.']])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where radius values of maximal inscribed spheres are stored'],
            ['EikonalSolutionArrayName','eikonalsolutionarray','str',1],
            ['EdgeArrayName','edgearray','str',1],
            ['EdgePCoordArrayName','edgepcoordarray','str',1],
            ['CostFunctionArrayName','costfunctionarray','str',1],
            ['DelaunayTessellation','delaunaytessellation','vtkUnstructuredGrid',1,'','','vmtkmeshwriter'],
            ['VoronoiDiagram','voronoidiagram','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['PoleIds','poleids','vtkIdList',1]])

    def PrintProgress(self,obj,event):
        self.OutputProgress(obj.GetProgress(),10)

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.CheckNonManifold:
            self.PrintLog('NonManifold check.')
            nonManifoldChecker = vmtkNonManifoldSurfaceChecker()
            nonManifoldChecker.Surface = self.Surface
            nonManifoldChecker.PrintError = self.PrintError
            nonManifoldChecker.Execute()

            if (nonManifoldChecker.NumberOfNonManifoldEdges > 0):
                self.PrintLog(nonManifoldChecker.Report)
                return

        if not self.vmtkRenderer and self.SeedSelectorName in ['pickpoint','openprofiles']:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.PrintLog('Cleaning surface.')
        surfaceCleaner = vtk.vtkCleanPolyData()
        surfaceCleaner.SetInputData(self.Surface)
        surfaceCleaner.Update()

        self.PrintLog('Triangulating surface.')
        surfaceTriangulator = vtk.vtkTriangleFilter()
        surfaceTriangulator.SetInputConnection(surfaceCleaner.GetOutputPort())
        surfaceTriangulator.PassLinesOff()
        surfaceTriangulator.PassVertsOff()
        surfaceTriangulator.Update()

        centerlineInputSurface = surfaceTriangulator.GetOutput()

        capCenterIds = None

        if self.SeedSelectorName in ['openprofiles', 'carotidprofiles', 'pickpoint', 'profileidlist']:
            self.PrintLog('Capping surface.')
            surfaceCapper = vtkvmtk.vtkvmtkCapPolyData()
            surfaceCapper.SetInputConnection(surfaceTriangulator.GetOutputPort())
            surfaceCapper.SetDisplacement(self.CapDisplacement)
            surfaceCapper.SetInPlaneDisplacement(self.CapDisplacement)
            surfaceCapper.Update()
            centerlineInputSurface = surfaceCapper.GetOutput()
            capCenterIds = surfaceCapper.GetCapCenterIds()

        if self.SeedSelector:
            pass
        elif self.SeedSelectorName:
            if self.SeedSelectorName == 'pickpoint':
                self.SeedSelector = vmtkPickPointSeedSelector()
                self.SeedSelector.vmtkRenderer = self.vmtkRenderer
                self.SeedSelector.Script = self
            elif self.SeedSelectorName == 'openprofiles':
                self.SeedSelector = vmtkOpenProfilesSeedSelector()
                self.SeedSelector.vmtkRenderer = self.vmtkRenderer
                self.SeedSelector.Script = self
                self.SeedSelector.SetSeedIds(surfaceCapper.GetCapCenterIds())
            elif self.SeedSelectorName == 'carotidprofiles':
                self.SeedSelector = vmtkCarotidProfilesSeedSelector()
                self.SeedSelector.SetSeedIds(surfaceCapper.GetCapCenterIds())
            elif self.SeedSelectorName == 'idlist':
                self.SeedSelector = vmtkIdListSeedSelector()
                self.SeedSelector.SourceIds = self.SourceIds
                self.SeedSelector.TargetIds = self.TargetIds
            elif self.SeedSelectorName == 'pointlist':
                self.SeedSelector = vmtkPointListSeedSelector()
                self.SeedSelector.SourcePoints = self.SourcePoints
                self.SeedSelector.TargetPoints = self.TargetPoints
            elif self.SeedSelectorName != 'profileidlist':
                self.PrintError("SeedSelectorName unknown (available: pickpoint, openprofiles, carotidprofiles, profileidlist, idlist, pointlist)")
                return
        else:
            self.PrintError('vmtkCenterlines error: either SeedSelector or SeedSelectorName must be specified')
            return

        if self.SeedSelector:
            self.SeedSelector.SetSurface(centerlineInputSurface)
            self.SeedSelector.InputInfo = self.InputInfo
            self.SeedSelector.InputText = self.InputText
            self.SeedSelector.OutputText = self.OutputText
            self.SeedSelector.PrintError = self.PrintError
            self.SeedSelector.PrintLog = self.PrintLog
            self.SeedSelector.Execute()

            inletSeedIds = self.SeedSelector.GetSourceSeedIds()
            outletSeedIds = self.SeedSelector.GetTargetSeedIds()
        else:
            inletSeedIds = vtk.vtkIdList()
            outletSeedIds = vtk.vtkIdList()
            for id in self.SourceIds:
                inletSeedIds.InsertNextId(id)
            if self.TargetIds:
                for id in self.TargetIds:
                    outletSeedIds.InsertNextId(id)
            else:
                for i in range(capCenterIds.GetNumberOfIds()):
                    if i not in self.SourceIds:
                        outletSeedIds.InsertNextId(i)

        self.PrintLog('Computing centerlines.')
        self.InputInfo('Computing centerlines...')

        centerlineFilter = vtkvmtk.vtkvmtkPolyDataCenterlines()
        centerlineFilter.SetInputData(centerlineInputSurface)
        if self.SeedSelectorName in ['openprofiles','carotidprofiles','profileidlist']:
            centerlineFilter.SetCapCenterIds(capCenterIds)
        centerlineFilter.SetSourceSeedIds(inletSeedIds)
        centerlineFilter.SetTargetSeedIds(outletSeedIds)
        centerlineFilter.SetRadiusArrayName(self.RadiusArrayName)
        centerlineFilter.SetCostFunction(self.CostFunction)
        centerlineFilter.SetFlipNormals(self.FlipNormals)
        centerlineFilter.SetAppendEndPointsToCenterlines(self.AppendEndPoints)
        centerlineFilter.SetSimplifyVoronoi(self.SimplifyVoronoi)
        if self.StopFastMarchingOnReachingTarget == True:
            if outletSeedIds.GetNumberOfIds() != 1:
                self.PrintError('Parameter Conflict: cannot enable "StopFastMarchingOnReachingTarget" when there is more then one target seed set.')
            else:
                centerlineFilter.SetStopFastMarchingOnReachingTarget(self.StopFastMarchingOnReachingTarget)
        if self.DelaunayTessellation is not None:
            centerlineFilter.GenerateDelaunayTessellationOff()
            centerlineFilter.SetDelaunayTessellation(self.DelaunayTessellation)
            centerlineFilter.SetDelaunayTolerance(self.DelaunayTolerance)
        if (self.VoronoiDiagram is not None) and (self.PoleIds is not None):
            centerlineFilter.GenerateVoronoiDiagramOff()
            centerlineFilter.SetVoronoiDiagram(self.VoronoiDiagram)
            centerlineFilter.SetPoleIds(self.PoleIds)
            if self.SimplifyVoronoi == True:
                centerlineFilter.SetSimplifyVoronoi(0)
                self.PrintLog('Note: requested behavior (SimplifyVoronoi = True) over-ridden.',1)
                self.PrintLog('Cannot simplify Voronoi Diagram when precomputed input is specified.',1)
        if self.UseTetGen==1:
            self.PrintLog('Running TetGen.')
            from vmtk import vmtkscripts
            surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
            surfaceToMesh.Surface = centerlineInputSurface
            surfaceToMesh.Execute()
            tetgen = vmtkscripts.vmtkTetGen()
            tetgen.Mesh = surfaceToMesh.Mesh
            tetgen.PLC = 1
            tetgen.NoMerge = 1
            tetgen.Quality = 0
            if self.TetGenDetectInter == 1:
                tetgen.DetectInter = 1
                tetgen.NoMerge = 0
            tetgen.OutputSurfaceElements = 0
            tetgen.Execute()
            centerlineFilter.GenerateDelaunayTessellationOff()
            centerlineFilter.SetDelaunayTessellation(tetgen.Mesh)
        centerlineFilter.SetCenterlineResampling(self.Resampling)
        centerlineFilter.SetResamplingStepLength(self.ResamplingStepLength)
        centerlineFilter.Update()

        self.Centerlines = centerlineFilter.GetOutput()
        self.VoronoiDiagram = centerlineFilter.GetVoronoiDiagram()
        self.DelaunayTessellation = centerlineFilter.GetDelaunayTessellation()
        self.PoleIds = centerlineFilter.GetPoleIds()

        self.EikonalSolutionArrayName = centerlineFilter.GetEikonalSolutionArrayName()
        self.EdgeArrayName = centerlineFilter.GetEdgeArrayName()
        self.EdgePCoordArrayName = centerlineFilter.GetEdgePCoordArrayName()
        self.CostFunctionArrayName = centerlineFilter.GetCostFunctionArrayName()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
