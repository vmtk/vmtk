#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshclipcenterlines.py,v $
## Language:  Python
## Date:      $$
## Version:   $$

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## Interactively clip a mesh using the distance to centerlines

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import vmtkscripts

from vmtk import pypes


class vmtkMeshClipCenterlines(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        #The orignal mesh
        self.Mesh = None
        #Preview of the resulting mesh
        self.PreviewMesh = None
        #Clipped part of the mesh
        self.ClippedMesh = None
        #Which mesh is displayed : 0 intial mesh, 1 preview mesh, 2 clipped mesh
        self.DisplayedMesh = 0
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Clipper = None
        #self.LineClipper = None
        self.InsideOut = 1
        #Type of polyball : 0 global, 1 local
        self.PolyBallType = 0

        self.CellEntityIdsArrayName = 'CellEntityIds'
        #Id of the first wall (mesh surface)
        self.WallCellEntityId = 1
        #Should we include the new surface cells
        self.IncludeSurfaceCells = 1
        #Tolerance used for reporjecting the cellentityid array
        self.Tolerance = -1.

        self.Centerlines = None
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.RadiusArray = None

        self.MeshActor = None
        self.CenterlinesActor = None

        #Spheres used to drive the radius array interpolation
        self.SpheresActor = None
        self.Spheres = vtk.vtkPolyData()
        self.SpheresIndices = vtk.vtkIdList()
        self.SpheresRadii = None
        self.CurrentSphereId = -1
        self.SphereWidget = None

        #Used to display the interpolated spheres
        self.InterpolatedGlyphs = None
        self.InterpolatedSpheresActor = None
        self.PolyBall = None
        #used to display the polyball function used for clipping
        self.PolyBallActor = None
        self.PolyBallResolution = [32, 32, 32]

        self.SetScriptName('vmtkmeshclipcenterlines')
        self.SetScriptDoc('interactively clip a mesh using the distance to centerlines')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1],
            ['PolyBallType','polyballtype','int',1,'','type of polyball used (0: global, 1: local)'],
            ['PolyBallResolution','polyballres','int',3,'','grid resolution for displaying the polyball'],
            ['InsideOut','insideout','bool',1,'','choose whether to clip the inside or outside of the polyball'],
            ['CellEntityIdsArrayName','entityidsarray','str',1,''],
            ['WallCellEntityId','wallid','int',1,'','id of the first surface cells in the entityids list'],
            ['Tolerance','tol','float',1,'','tolerance used to reproject the entity ids on the new surface']
            #['IncludeSurfaceCells','includesurfacecells','bool',0,'','include new surface cells in output mesh'],
            #['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['ClippedMesh','clippedmesh','vtkUnstructuredGrid',1,'','the clipped mesh','vmtkmeshwriter']
            ])

    def InitializeSpheres(self):
        #Reset the sphere array
        self.Spheres.Initialize()
        spherePoints = vtk.vtkPoints()
        self.Spheres.SetPoints(spherePoints)
        self.Spheres.GetPointData().Initialize()
        self.SpheresIndices.Initialize()
        self.SpheresRadii = vtk.vtkDoubleArray()
        self.SpheresRadii.SetName("SpheresRadii")
        self.Spheres.GetPointData().SetScalars(self.SpheresRadii)
        self.CurrentSphereId = -1
        #Hide the widget
        self.SphereWidget.Off()
        #Disable scaling
        self.InterpolatedGlyphs.SetScaleModeToDataScalingOff()
        self.InterpolatedGlyphs.SetScaleFactor(0.)
        #reset the polyball
        self.PolyBall = None

    def PlaceSphere(self):
        if self.CurrentSphereId == -1:
            return
        self.SphereWidget.SetCenter(self.Spheres.GetPoint(self.CurrentSphereId))
        self.SphereWidget.SetRadius(self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId))
        self.SphereWidget.On()

    def SphereCallback(self,widget,event_string):
        if self.CurrentSphereId == -1:
            return
        minRadius = self.Centerlines.GetLength()*0.001
        if self.SphereWidget.GetRadius() < minRadius:
            self.SphereWidget.SetRadius(minRadius)
        self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,self.SphereWidget.GetRadius())
        self.Spheres.Modified()

    def InterpolateRadius(self):
      #if we have no values return
      if self.SpheresRadii.GetNumberOfTuples() == 0:
        return;
      interpolator = vtkvmtk.vtkvmtkCenterlineInterpolateArray()
      interpolator.SetInputData(self.Centerlines)
      interpolator.SetValues(self.SpheresRadii)
      interpolator.SetValuesIds(self.SpheresIndices)
      interpolator.SetInterpolatedArrayName("InterpolatedRadius")
      interpolator.Update()
      self.Centerlines = interpolator.GetOutput()
      self.Centerlines.GetPointData().SetActiveScalars("InterpolatedRadius")
      self.InterpolatedGlyphs.SetInputData(self.Centerlines)
      #enable scaling
      self.InterpolatedGlyphs.SetScaleModeToScaleByScalar()
      self.InterpolatedGlyphs.SetScaleFactor(1)

    def BuildPolyBall(self):
        #if we have no values return
        if self.SpheresRadii.GetNumberOfTuples() == 0:
            return
        #interpolate the radius array
        self.InterpolateRadius()
        if self.PolyBallType==1:
          self.PolyBall = vtkvmtk.vtkvmtkPolyBallLine2()
        else:
          self.PolyBall = vtkvmtk.vtkvmtkPolyBallLine()
        self.PolyBall.SetInputData(self.Centerlines)
        self.PolyBall.SetUseRadiusInformation(1)
        self.PolyBall.SetPolyBallRadiusArrayName("InterpolatedRadius")

    def BuildPolyBallSurface(self):
        #Build a surface for displaying the polyball
        if self.PolyBall == None:
            return

        #Sample the polyball
        sampler = vtk.vtkSampleFunction()
        sampler.SetImplicitFunction(self.PolyBall)

        #Set the bounds to be slightly larger than those of the mesh
        meshBounds = self.Mesh.GetBounds()
        meshCenter = self.Mesh.GetCenter()
        polyBallBounds = [0, 0, 0, 0, 0, 0]
        for i in range(0,3):
            length = 1.2*(meshBounds[2*i+1] - meshCenter[i])
            polyBallBounds[2*i] = meshCenter[i] - length
            polyBallBounds[2*i+1] = meshCenter[i] + length

        sampler.SetModelBounds(polyBallBounds)
        sampler.SetSampleDimensions(self.PolyBallResolution)
        sampler.ComputeNormalsOff()
        sampler.Update()

        #Extract the isosurface at 0
        contour = vtk.vtkContourFilter()
        contour.SetInputConnection(sampler.GetOutputPort())
        contour.SetValue(0,0.)
        contour.Update()

        #Set the new model as the mapper input
        self.PolyBallActor.GetMapper().SetInputConnection(contour.GetOutputPort())
        #self.PolyBallActor.VisibilityOn()

    def ClipMesh(self):
        #if we have no values simply return the original mesh
        if self.SpheresRadii.GetNumberOfTuples() == 0:
            self.PreviewMesh = self.Mesh
            self.ClippedMesh = None
            return

        self.BuildPolyBall()

        #Update the polyball model if necessary
        if self.PolyBallActor.GetVisibility():
            self.BuildPolyBallSurface()

        self.Clipper.SetClipFunction(self.PolyBall)
        self.Clipper.Update()
        self.PreviewMesh = self.Clipper.GetOutput()
        self.ClippedMesh = self.Clipper.GetClippedOutput()

    #Clip the mesh using vtkvmtkClipDataSetLine
    #def ClipMeshLine(self):
        ##if we have no values simply return the original mesh
        #if self.SpheresRadii.GetNumberOfTuples() == 0:
            #self.PreviewMesh = self.Mesh
            #self.ClippedMesh = None
            #return

        #self.BuildPolyBall()

        ##Update the polyball model if necessary
        #if self.PolyBallActor.GetVisibility():
            #self.BuildPolyBallSurface()

        #lineIntersector = vtkvmtk.vtkvmtkPolyBallLineIntersector()
        #lineIntersector.SetImplicitFunction(self.PolyBall)
        #self.LineClipper.SetLineIntersector(lineIntersector)
        #self.LineClipper.Update()
        #self.PreviewMesh = self.LineClipper.GetOutput()
        #self.ClippedMesh = self.LineClipper.GetClippedOutput()

    #Recreate the surface cells on the clipped and project the cell entity ids
    def CreateSurfaceCells(self,inMesh):
        #Remove the surface cells from the mesh
        cellDimFilter = vtkvmtk.vtkvmtkCellDimensionFilter()
        cellDimFilter.SetInputData(inMesh)
        cellDimFilter.ThresholdByUpper(3)
        cellDimFilter.Update()
        volumetricMesh = cellDimFilter.GetOutput()

        #Get new surface cells
        geomFilter = vtk.vtkGeometryFilter()
        geomFilter.SetInputConnection(cellDimFilter.GetOutputPort())
        geomFilter.Update()
        newSurfaceCells = geomFilter.GetOutput()

        #If the celEntityIdArray exist, project the original entity ids
        cellEntityIdsArray = newSurfaceCells.GetCellData().GetArray(self.CellEntityIdsArrayName)
        if (cellEntityIdsArray != None):
            #Convert the surface cells to poly data
            surfaceCellsToSurface = vmtkscripts.vmtkMeshToSurface()
            surfaceCellsToSurface.Mesh = newSurfaceCells
            surfaceCellsToSurface.Execute()

            #Get the original surface cells
            meshThreshold = vtk.vtkThreshold()
            meshThreshold.SetInputData(self.Mesh)
            meshThreshold.ThresholdByUpper(self.WallCellEntityId+0.5)
            meshThreshold.SetInputArrayToProcess(0,0,0,1,self.CellEntityIdsArrayName)
            meshThreshold.Update()

            meshToSurface = vmtkscripts.vmtkMeshToSurface()
            meshToSurface.Mesh = meshThreshold.GetOutput()
            meshToSurface.Execute()

            #Project the entity ids form the old surface cells to the new surface cells
            #TODO: This is hackish(need for a tolerance), find a beeter way
            projector = vtkvmtk.vtkvmtkSurfaceProjectCellArray()
            projector.SetInputData(surfaceCellsToSurface.Surface)
            projector.SetReferenceSurface(meshToSurface.Surface)
            projector.SetProjectedArrayName(self.CellEntityIdsArrayName)
            projector.SetDefaultValue(self.WallCellEntityId)
            projector.SetDistanceTolerance(self.Tolerance)
            projector.Update()

            #Convert the surface cells back to unstructured grid
            surfaceToMesh = vmtkscripts.vmtkSurfaceToMesh()
            surfaceToMesh.Surface = projector.GetOutput()
            surfaceToMesh.Execute()

            newSurfaceCells = surfaceToMesh.Mesh

        #append the new surface cells to the volumetric elements
        appendFilter = vtkvmtk.vtkvmtkAppendFilter()
        appendFilter.AddInput(volumetricMesh)
        appendFilter.AddInput(newSurfaceCells)
        appendFilter.Update()

        return appendFilter.GetOutput()

    def LCallback(self,obj):
        self.DisplayedMesh = (self.DisplayedMesh+1)%3
        if self.DisplayedMesh == 0:
            self.MeshActor.GetMapper().SetInputData(self.Mesh)
        elif self.DisplayedMesh == 1:
            self.MeshActor.GetMapper().SetInputData(self.PreviewMesh)
        elif self.DisplayedMesh == 2:
            self.MeshActor.GetMapper().SetInputData(self.ClippedMesh)
        self.vmtkRenderer.RenderWindow.Render()

    def ClipCallback(self, obj):
        self.ClipMesh()
        if self.DisplayedMesh == 1:
            self.MeshActor.GetMapper().SetInputData(self.PreviewMesh)
        elif self.DisplayedMesh == 2:
           self.MeshActor.GetMapper().SetInputData(self.ClippedMesh)
        self.vmtkRenderer.RenderWindow.Render()

    #def MCallback(self, obj)
        #self.ClipMeshLine()
        #if self.DisplayedMesh == 1:
            #self.MeshActor.GetMapper().SetInputData(self.PreviewMesh)
        #elif self.DisplayedMesh == 2:
            #self.MeshActor.GetMapper().SetInputData(self.ClippedMesh)
        #self.vmtkRenderer.RenderWindow.Render()

    def SpaceCallback(self, obj):
        #Disable the sphere Widget so as not to pick it
        self.SphereWidget.Off()
        #add a new sphere
        picker = vtk.vtkPointPicker()
        picker.SetTolerance(1E-3 * self.Centerlines.GetLength())
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        #eventPosition = obj.GetEventPosition()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        if result == 0:
            #Reenable the sphere widget
            self.SphereWidget.On()
            return
        pickId = picker.GetPointId()
        #Only insert a new sphere if this id is not already in the list
        if self.SpheresIndices.IsId(pickId) == -1:
            self.CurrentSphereId = self.Spheres.GetPoints().InsertNextPoint(self.Centerlines.GetPoint(pickId))
            self.SpheresIndices.InsertNextId(pickId)
            interpolatedArray = self.Centerlines.GetPointData().GetArray("InterpolatedRadius")
            if interpolatedArray and (interpolatedArray.GetValue(pickId) != 0.):
                self.SpheresRadii.InsertNextValue(interpolatedArray.GetValue(pickId))
            elif self.RadiusArray:
                self.SpheresRadii.InsertNextValue(self.RadiusArray.GetValue(pickId))
            else:
                self.SpheresRadii.InsertNextValue(self.Centerlines.GetLength()*0.01)
            self.Spheres.Modified()
            self.PlaceSphere()
        #Reenable the sphere widget
        self.SphereWidget.On()
        self.vmtkRenderer.RenderWindow.Render()

    def KeyPressed(self,obj,event):
         key = obj.GetKeySym()
         if key == 'space':
             #Disable the sphere Widget so as not to pick it
             self.SphereWidget.Off()
             #add a new sphere
             picker = vtk.vtkPointPicker()
             picker.SetTolerance(1E-3 * self.Centerlines.GetLength())
             eventPosition = obj.GetEventPosition()
             result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
             if result == 0:
                 #Reenable the sphere widget
                 self.SphereWidget.On()
                 return
             pickId = picker.GetPointId()
             #Only insert a new sphere if this id is not already in the list
             if self.SpheresIndices.IsId(pickId) == -1:
                 self.CurrentSphereId = self.Spheres.GetPoints().InsertNextPoint(self.Centerlines.GetPoint(pickId))
                 self.SpheresIndices.InsertNextId(pickId)
                 interpolatedArray = self.Centerlines.GetPointData().GetArray("InterpolatedRadius")
                 if interpolatedArray and (interpolatedArray.GetValue(pickId) != 0.):
                     self.SpheresRadii.InsertNextValue(interpolatedArray.GetValue(pickId))
                 elif self.RadiusArray:
                     self.SpheresRadii.InsertNextValue(self.RadiusArray.GetValue(pickId))
                 else:
                     self.SpheresRadii.InsertNextValue(self.Centerlines.GetLength()*0.01)
                 self.Spheres.Modified()
                 self.PlaceSphere()
            #Reenable the sphere widget
             self.SphereWidget.On()
             self.vmtkRenderer.RenderWindow.Render()

    def PlusCallback(self, obj):
        if self.CurrentSphereId != -1:
            #increase sphere radius
            newval = self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId) + self.Centerlines.GetLength()*0.01
            self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,newval)
            self.Spheres.Modified()
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def MinusCallback(self, obj):
        if self.CurrentSphereId != -1:
            #decrease sphere radius
            newval = self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId) - self.Centerlines.GetLength()*0.01
            if newval> 0:
                self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,newval)
                self.Spheres.Modified()
                self.PlaceSphere()
                self.vmtkRenderer.RenderWindow.Render()

    def NextSphereCallback(self, obj):
        if self.CurrentSphereId != -1:
            #skip to next sphere
            self.CurrentSphereId = (self.CurrentSphereId + 1) % self.Spheres.GetNumberOfPoints();
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def ProviousSphereCallback(self, obj):
        if self.CurrentSphereId != -1:
            #skip to previous sphere
            self.CurrentSphereId = (self.CurrentSphereId - 1) % self.Spheres.GetNumberOfPoints();
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def UndoCallback(self, obj):
        #undo
        self.InitializeSpheres()
        self.Spheres.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def DeleteSphereCallback(self, obj):
        if self.CurrentSphereId != -1:
            #delete the current sphere
            #if no spheres would be left initialize
            if self.Spheres.GetNumberOfPoints() == 1:
                self.InitializeSpheres()
            else:
                #Copy the spheres into a new structure
                oldPoints = vtk.vtkPoints()
                oldPoints.DeepCopy(self.Spheres.GetPoints())
                self.Spheres.Initialize()
                spherePoints = vtk.vtkPoints()
                for i in range(0,oldPoints.GetNumberOfPoints()):
                    if i != self.CurrentSphereId:
                        spherePoints.InsertNextPoint(oldPoints.GetPoint(i))
                self.Spheres.SetPoints(spherePoints)
                self.SpheresRadii.RemoveTuple(self.CurrentSphereId)
                self.Spheres.GetPointData().SetScalars(self.SpheresRadii)
                self.SpheresIndices.DeleteId(self.SpheresIndices.GetId(i))
                self.CurrentSphereId = (self.CurrentSphereId + 1) % self.Spheres.GetNumberOfPoints()
                self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def InterpolatedSphereCallback(self, obj):
        #Display the interpolated spheres
        self.InterpolatedSpheresActor.SetVisibility(not self.InterpolatedSpheresActor.GetVisibility())
        self.vmtkRenderer.RenderWindow.Render()

    def PolyballSurfaceCallback(self, obj):
        #Display/hide the polyball surface
        if self.PolyBallActor.GetVisibility():
            self.PolyBallActor.VisibilityOff()
        elif self.PolyBall != None:
            self.BuildPolyBallSurface()
            self.PolyBallActor.VisibilityOn()
            self.vmtkRenderer.RenderWindow.Render()

    def MeshVisalizationCallback(self, obj):
        #Switch between transparent/opaque mesh for better visualisation
        if self.MeshActor.GetProperty().GetOpacity() == 0.25:
            self.MeshActor.GetProperty().SetOpacity(1.)
        else:
            self.MeshActor.GetProperty().SetOpacity(0.25)
        self.vmtkRenderer.RenderWindow.Render()

    def PolyballVisualizationCallback(self, obj):
        #Switch between transparent/opaque polyball for better visualisation
        if self.PolyBallActor.GetProperty().GetOpacity() == 0.25:
            self.PolyBallActor.GetProperty().SetOpacity(1.)
            self.MeshActor.VisibilityOff()
        else:
            self.PolyBallActor.GetProperty().SetOpacity(0.25)
            self.MeshActor.VisibilityOn()
        self.vmtkRenderer.RenderWindow.Render()

    def PolyballTypeCallback(self, obj):
        self.PolyBallType = (self.PolyBallType + 1)%2

    def Display(self):

        self.vmtkRenderer.RenderWindowInteractor.Initialize()

        #self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)
        self.vmtkRenderer.AddKeyBinding('f','Change Polyball type',self.PolyballTypeCallback)
        self.vmtkRenderer.AddKeyBinding('y','Switch between transparent/opaque polyball',self.PolyballVisualizationCallback)
        self.vmtkRenderer.AddKeyBinding('w','Switch between transparent/opaque mesh',self.MeshVisalizationCallback)
        self.vmtkRenderer.AddKeyBinding('b','Display/hide the polyball surface',self.PolyballSurfaceCallback)
        self.vmtkRenderer.AddKeyBinding('o','Display the interpolated spheres',self.InterpolatedSphereCallback)
        self.vmtkRenderer.AddKeyBinding('d','Delete the current sphere',self.DeleteSphereCallback)
        self.vmtkRenderer.AddKeyBinding('u','Undo',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('v','Skip to previous sphere',self.ProviousSphereCallback)
        self.vmtkRenderer.AddKeyBinding('n','skip to next sphere',self.NextSphereCallback)
        self.vmtkRenderer.AddKeyBinding('minus','Decrease sphere radius',self.MinusCallback)
        self.vmtkRenderer.AddKeyBinding('plus','Increase sphere radius',self.PlusCallback)
        self.vmtkRenderer.AddKeyBinding('space','Disable the sphere Widget',self.SpaceCallback)
        self.vmtkRenderer.AddKeyBinding('c','Clip Mesh',self.ClipCallback)
        self.vmtkRenderer.AddKeyBinding('l','Change Displayed Mesh',self.LCallback)

        self.vmtkRenderer.Render()
        #self.vmtkRenderer.RenderWindowInteractor.Start()

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        if (self.Centerlines == None):
            self.PrintError('Error: no Centerlines')

        #Save the centerlines
        previousCenterlines = self.Centerlines

        cleaner = vtk.vtkCleanPolyData()
        cleaner.SetInputData(self.Centerlines)
        cleaner.Update()
        self.Centerlines = cleaner.GetOutput()

        if self.Tolerance == -1:
            self.Tolerance = 0.000001*self.Mesh.GetLength()

        if self.RadiusArrayName != '':
            self.RadiusArray = self.Centerlines.GetPointData().GetArray(self.RadiusArrayName)
            if self.RadiusArray == None:
                self.PrintError('Error : could not find radius array')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        meshMapper = vtk.vtkDataSetMapper()
        meshMapper.SetInputData(self.Mesh)
        meshMapper.ScalarVisibilityOff()
        self.MeshActor = vtk.vtkActor()
        self.MeshActor.SetMapper(meshMapper)
        self.MeshActor.GetProperty().SetOpacity(0.25)
        self.MeshActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.MeshActor)

        centerlinesMapper = vtk.vtkDataSetMapper()
        centerlinesMapper.SetInputData(self.Centerlines)
        centerlinesMapper.ScalarVisibilityOff()
        self.CenterlinesActor = vtk.vtkActor()
        self.CenterlinesActor.SetMapper(centerlinesMapper)
        self.vmtkRenderer.Renderer.AddActor(self.CenterlinesActor)

        glyphs = vtk.vtkGlyph3D()
        glyphSource = vtk.vtkSphereSource()
        glyphSource.SetRadius(1)
        glyphs.SetInputData(self.Spheres)
        glyphs.SetSourceConnection(glyphSource.GetOutputPort())
        glyphs.SetScaleModeToScaleByScalar()
        glyphs.SetScaleFactor(1.)
        glyphMapper = vtk.vtkPolyDataMapper()
        glyphMapper.SetInputConnection(glyphs.GetOutput())
        glyphMapper.ScalarVisibilityOff()
        self.SpheresActor = vtk.vtkActor()
        self.SpheresActor.SetMapper(glyphMapper)
        self.SpheresActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.SpheresActor.GetProperty().SetOpacity(0.25)
        self.SpheresActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.SpheresActor)

        self.InterpolatedGlyphs = vtk.vtkGlyph3D()
        interpolatedGlyphSource = vtk.vtkSphereSource()
        interpolatedGlyphSource.SetRadius(1)
        self.InterpolatedGlyphs.SetInputData(self.Centerlines)
        self.InterpolatedGlyphs.SetSourceConnection(interpolatedGlyphSource.GetOutputPort())
        #scaling is off for now
        self.InterpolatedGlyphs.SetScaleModeToDataScalingOff()
        self.InterpolatedGlyphs.SetScaleFactor(0.)
        interpolatedGlyphMapper = vtk.vtkPolyDataMapper()
        interpolatedGlyphMapper.SetInputConnection(self.InterpolatedGlyphs.GetOutputPort())
        interpolatedGlyphMapper.ScalarVisibilityOff()
        self.InterpolatedSpheresActor = vtk.vtkActor()
        self.InterpolatedSpheresActor.SetMapper(interpolatedGlyphMapper)
        self.InterpolatedSpheresActor.GetProperty().SetColor(0.0,1.0,0.0)
        self.InterpolatedSpheresActor.GetProperty().SetOpacity(0.25)
        self.InterpolatedSpheresActor.PickableOff()
        self.InterpolatedSpheresActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.InterpolatedSpheresActor)

        polyBallMapper = vtk.vtkPolyDataMapper()
        polyBallMapper.ScalarVisibilityOff()
        self.PolyBallActor = vtk.vtkActor()
        self.PolyBallActor.SetMapper(polyBallMapper)
        self.PolyBallActor.GetProperty().SetColor(0.0,1.0,0.0)
        self.PolyBallActor.GetProperty().SetOpacity(0.25)
        self.PolyBallActor.PickableOff()
        self.PolyBallActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.PolyBallActor)

        self.SphereWidget = vtk.vtkSphereWidget()
        self.SphereWidget.TranslationOff()
        self.SphereWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.SphereWidget.AddObserver("InteractionEvent", self.SphereCallback)

        self.Clipper = vtk.vtkClipDataSet()
        self.Clipper.SetInputData(self.Mesh)
        self.Clipper.SetInsideOut(self.InsideOut)
        self.Clipper.GenerateClippedOutputOn()

        #self.LineClipper = vtkvmtk.vtkvmtkClipDataSetLine()
        #self.LineClipper.SetInputData(self.Mesh)
        #self.LineClipper.SetInsideOut(self.InsideOut)
        #self.LineClipper.GenerateClippedOutputOn()

        self.InitializeSpheres()

        self.PreviewMesh = self.Mesh

        self.Display()

        self.PolyBallActor.VisibilityOff()
        self.ClipMesh()

        if self.ClippedMesh == None:
            #return an empty mesh
            self.ClippedMesh = vtk.vtkUnstructuredGrid()
        elif self.IncludeSurfaceCells:
            #Create the surface cells
            self.PreviewMesh = self.CreateSurfaceCells(self.PreviewMesh)
            self.ClippedMesh = self.CreateSurfaceCells(self.ClippedMesh)

        self.Mesh = self.PreviewMesh

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

        #Restore the centerlines
        self.Centerlines = previousCenterlines


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
