#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceendclipper.py,v $
## Language:  Python
## Date:      $Date: 2015/12/01 14:45:13 $
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Simone Manini
##       Orobix Srl
## And adapted and modified by
##       Kurt Sansom

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
        self._MainBodySeedIds = vtk.vtkIdList()
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

    def GetMainBodySeedIds(self):
        return self._MainBodySeedIds

    def GetSourceSeedIds(self):
        return self._SourceSeedIds

    def GetTargetSeedIds(self):
        return self._TargetSeedIds

    def Execute(self):
        pass


class vmtkPickPointSeedSelector(vmtkSeedSelector):

    def __init__(self):
        vmtkSeedSelector.__init__(self)
        self.PickedSeedIds = vtk.vtkIdList()
        self.PickedSeeds = vtk.vtkPolyData()
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Script = None
        self.SetOutlets = 0

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

        self._MainBodySeedIds.Initialize()
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

        self.InputInfo('Please position the mouse and press space to add a point on the main body of the model, \'u\' to undo\n')

        any = 0
        while any == 0:
            self.InitializeSeeds()
            self.vmtkRenderer.Render()
            any = self.PickedSeedIds.GetNumberOfIds()
        self._MainBodySeedIds.DeepCopy(self.PickedSeedIds)

        if( self.SetOutlets ):
            self.InputInfo('Please position the mouse and press space to add point on vessel wall near each Inlet location, \'u\' to undo\n')
        else:
            self.InputInfo('Please position the mouse and press space to add points at clip locations, \'u\' to undo\n')

        any = 0
        while any == 0:
            self.InitializeSeeds()
            self.vmtkRenderer.Render()
            any = self.PickedSeedIds.GetNumberOfIds()
        self._SourceSeedIds.DeepCopy(self.PickedSeedIds)

        if(self.SetOutlets):
            self.InputInfo('Please position the mouse and press space to add point on vessel wall near each Outlet location, \'u\' to undo\n')

            any = 0
            while any == 0:
                self.InitializeSeeds()
                self.vmtkRenderer.Render()
                any = self.PickedSeedIds.GetNumberOfIds()
            self._TargetSeedIds.DeepCopy(self.PickedSeedIds)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


class vmtkSurfaceEndClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.CenterlineNormals = 0
        self.FrenetTangentArrayName = 'FrenetTangent'

        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.SeedSelector = None
        self.SeedSelectorName = 'pointlist'

        self.SourcePoints = []
        self.TargetPoints = []

        self.CleanOutput = 1

        self.SetScriptName('vmtksurfaceendclipper')
        self.SetScriptDoc('interactively clip a tubular surface with normals estimated at seed locations. It is \
                           recomended that the option to clip based on centerlines normals is enabled to increase \
                           the accuracy of the clipping. If desired, a centerline containing the Frenet Tangent Array \
                           must be fed in along with the surface. This is generated by passing a centerline through the \
                           vmtkcenterlinegeometry script.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CenterlineNormals','centerlinenormal','bool',1,'','toggle using centerline tangent for normal estimation'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['FrenetTangentArrayName','frenettangentarray','str',1,'','name of the array where centerline tangent vectors of the Frenet reference system are stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if (self.CenterlineNormals) and (self.Centerlines == None):
            self.PrintError('Error: No input centerlines.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.SeedSelector = vmtkPickPointSeedSelector()
        self.SeedSelector.vmtkRenderer = self.vmtkRenderer
        self.SeedSelector.Script = self

        if (self.CenterlineNormals):
            self.SeedSelector.SetOutlets = 1
        self.SeedSelector.SetSurface(self.Surface)
        self.SeedSelector.InputInfo = self.InputInfo
        self.SeedSelector.InputText = self.InputText
        self.SeedSelector.OutputText = self.OutputText
        self.SeedSelector.PrintError = self.PrintError
        self.SeedSelector.PrintLog = self.PrintLog
        self.SeedSelector.Execute()

        mainBodySeedIds = self.SeedSelector.GetMainBodySeedIds()
        mainBodySeedId = mainBodySeedIds.GetId(0)
        mainBodyPoint = self.Surface.GetPoint(mainBodySeedId)

        clipSeedIds = self.SeedSelector.GetSourceSeedIds()

        surfaceCleaner = vtk.vtkCleanPolyData()
        surfaceCleaner.SetInputData(self.Surface)
        surfaceCleaner.Update()

        surfaceTriangulator = vtk.vtkTriangleFilter()
        surfaceTriangulator.SetInputConnection(surfaceCleaner.GetOutputPort())
        surfaceTriangulator.PassLinesOff()
        surfaceTriangulator.PassVertsOff()
        surfaceTriangulator.Update()

        clippedSurface = surfaceTriangulator.GetOutput()

        n_seeds = clipSeedIds.GetNumberOfIds() # get the number of inlets
        if (self.CenterlineNormals):
            targetSeedIds = self.SeedSelector.GetTargetSeedIds()
            for i in range(targetSeedIds.GetNumberOfIds()):
                clipSeedIds.InsertNextId(targetSeedIds.GetId(i))
            locator_ctr = vtk.vtkPointLocator()
            locator_ctr.SetDataSet(self.Centerlines)
            locator_ctr.BuildLocator()

        for i in range(clipSeedIds.GetNumberOfIds()):

            seedId = clipSeedIds.GetId(i)
            seedPoint = self.Surface.GetPoint(seedId)

            # the frenet tangent must be inverted as it is directed outwards
            # the seam filter requires an inward facing normal
            if (self.CenterlineNormals):
                centerlinePointId = locator_ctr.FindClosestPoint(seedPoint)
                origin = self.Centerlines.GetPoint(centerlinePointId)
                normal = self.Centerlines.GetPointData().GetArray(self.FrenetTangentArrayName).GetTuple(centerlinePointId)
                if( i >= n_seeds ):
                    # invert outward facing normals
                    normal = tuple( -p for p in normal)
            else:
                locator = vtk.vtkPointLocator()
                locator.SetDataSet(clippedSurface)
                locator.BuildLocator()
                seedPointId = locator.FindClosestPoint(seedPoint)

                planeEstimator = vtkvmtk.vtkvmtkPolyDataNormalPlaneEstimator()
                planeEstimator.SetInputData(clippedSurface)
                planeEstimator.SetOriginPointId(seedPointId)
                planeEstimator.Update()
                origin = planeEstimator.GetOrigin()
                normal = planeEstimator.GetNormal()

            plane = vtk.vtkPlane()
            plane.SetOrigin(origin)
            plane.SetNormal(normal)

            seamFilter = vtkvmtk.vtkvmtkTopologicalSeamFilter()
            seamFilter.SetInputData(clippedSurface)
            seamFilter.SetClosestPoint(seedPoint)
            seamFilter.SetSeamScalarsArrayName("SeamScalars")
            seamFilter.SetSeamFunction(plane)

            clipper = vtk.vtkClipPolyData()
            clipper.SetInputConnection(seamFilter.GetOutputPort())
            clipper.GenerateClipScalarsOff()
            clipper.GenerateClippedOutputOn()

            connectivity = vtk.vtkPolyDataConnectivityFilter()
            connectivity.SetInputConnection(clipper.GetOutputPort())
            connectivity.SetExtractionModeToClosestPointRegion()
            connectivity.SetClosestPoint(mainBodyPoint)

            surfaceCleaner = vtk.vtkCleanPolyData()
            surfaceCleaner.SetInputConnection(connectivity.GetOutputPort())
            surfaceCleaner.Update()

            surfaceTriangulator = vtk.vtkTriangleFilter()
            surfaceTriangulator.SetInputConnection(surfaceCleaner.GetOutputPort())
            surfaceTriangulator.PassLinesOff()
            surfaceTriangulator.PassVertsOff()
            surfaceTriangulator.Update()

            #clippedSurface = vtk.vtkPolyData()
            #clippedSurface.DeepCopy(surfaceTriangulator.GetOutput())
            clippedSurface = surfaceTriangulator.GetOutput()

        self.Surface = clippedSurface

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
