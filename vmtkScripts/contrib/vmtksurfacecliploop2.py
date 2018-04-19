## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecliploop2.py,v $
## Language:  Python
## Date:      $Date: 2018/04/12 16:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Elena Faggiano (elena.faggiano@gmail.com)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
from __future__ import print_function # NEED TO STAY AS TOP IMPORT
import vtk
import sys
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


class vmtkSurfaceClipLoop2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Loop = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtksurfacecliploop')
        self.SetScriptDoc('Clip input surface using a loop')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Loop','i2','vtkPolyData',1,'','the input loop','vmtksurfacereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ])

    def Execute(self):

        from vmtk import vmtkscripts
        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.Loop == None:
            self.PrintError('Error: no Loop.')

        self.SeedSelector = vmtkPickPointSeedSelector()
        self.SeedSelector.vmtkRenderer = self.vmtkRenderer
        self.SeedSelector.Script = self

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

        surfaceCleaner = vtk.vtkCleanPolyData()
        surfaceCleaner.SetInputData(self.Surface)
        surfaceCleaner.Update()

        surfaceTriangulator = vtk.vtkTriangleFilter()
        surfaceTriangulator.SetInputConnection(surfaceCleaner.GetOutputPort())
        surfaceTriangulator.PassLinesOff()
        surfaceTriangulator.PassVertsOff()
        surfaceTriangulator.Update()

        clippedSurface = surfaceTriangulator.GetOutput()

        select = vtk.vtkImplicitSelectionLoop()
        select.SetLoop(self.Loop.GetPoints())
        normal = [0.0,0.0,0.0]
        centroid = [0.0,0.0,0.0]
        vtk.vtkPolygon().ComputeNormal(self.Loop.GetPoints(),normal)

        #compute centroid and check normals
        p = [0.0,0.0,0.0]
        for i in range (self.Loop.GetNumberOfPoints()):
            p = self.Loop.GetPoint(i)
            centroid[0] += p[0]
            centroid[1] += p[1]
            centroid[2] += p[2]
        centroid[0] = centroid[0] / self.Loop.GetNumberOfPoints()
        centroid[1] = centroid[1] / self.Loop.GetNumberOfPoints()
        centroid[2] = centroid[2] / self.Loop.GetNumberOfPoints()
        print("loop centroid", centroid)

        locator = vtk.vtkPointLocator()
        locator.SetDataSet(self.Surface)
        locator.AutomaticOn()
        locator.BuildLocator()
        idsurface = locator.FindClosestPoint(centroid)

        if (self.Surface.GetPointData().GetNormals() == None):
            normalsFilter = vmtkscripts.vmtkSurfaceNormals()
            normalsFilter.Surface = self.Surface
            normalsFilter.NormalsArrayName = 'Normals'
            normalsFilter.Execute()
            self.Surface = normalsFilter.Surface
        normalsurface = [0.0,0.0,0.0]
        self.Surface.GetPointData().GetNormals().GetTuple(idsurface,normalsurface)
        print("loop normal: ", normal)
        print("surface normal inside the loop: ", normalsurface)
        check = vtk.vtkMath.Dot(normalsurface,normal)
        if check < 0:
            normal[0] = - normal[0]
            normal[1] = - normal[1]
            normal[2] = - normal[2]

        #compute plane
        proj = float(vtk.vtkMath.Dot(self.Loop.GetPoint(0),normal))
        point = [0.0,0.0,0.0]
        self.Loop.GetPoint(0,point)
        for i in range (self.Loop.GetNumberOfPoints()):
            tmp = vtk.vtkMath.Dot(self.Loop.GetPoint(i),normal)
            if tmp < proj:
                proj = tmp
                self.Loop.GetPoint(i,point)
        origin = [0.0,0.0,0.0]
        origin[0] = point[0] #- normal[0]
        origin[1] = point[1] #- normal[1]
        origin[2] = point[2] #- normal[2]
        plane=vtk.vtkPlane()
        plane.SetNormal(normal[0],normal[1],normal[2])
        plane.SetOrigin(origin[0],origin[1],origin[2])

        seamFilter = vtkvmtk.vtkvmtkTopologicalSeamFilter()
        seamFilter.SetInputData(self.Surface)
        seamFilter.SetClosestPoint(idsurface)
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

        #set bool
        Bool = vtk.vtkImplicitBoolean()
        Bool.SetOperationTypeToDifference()
        Bool.AddFunction(select)
        Bool.AddFunction(plane)

        clipper=vtk.vtkClipPolyData()
        clipper.SetInputData()
        clipper.SetClipFunction(Bool)
        clipper.GenerateClippedOutputOn()
        clipper.InsideOutOff()
        clipper.Update()

        self.Surface = clipper.GetOutput()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
