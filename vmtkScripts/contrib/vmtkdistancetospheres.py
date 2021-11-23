#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkdistancetospheres.py,v $
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

## This class computes the euclidean from a set of user-selected spheres to a surface

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkDistanceToSpheres(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.DistanceToSpheresArrayName = 'DistanceToSpheres'
        self.DistanceOffset = 0.
        self.DistanceScale = 1.
        self.MinDistance = 0.
        self.MaxDistance = -1.
        self.Spheres = vtk.vtkPolyData()
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.DisplayArray = False
        self.SurfaceMapper = None
        self.CurrentSphereId = -1
        self.SphereWidget = None
        self.Opacity = 1.
        self.SpheresActor = None
        self.ScalarBarActor = None
        self.InteractionMode = 0
        self.ExamineSurface = None
        self.ExamineSpheres = vtk.vtkPolyData()
        self.ExamineSpheresActor = None
        self.ExamineText = None
        self.ParametersChanged = False

        self.SetScriptName('vmtkdistancetospheres')
        self.SetScriptDoc('This computes the euclidean from a set of user-selected spheres to a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['DistanceToSpheresArrayName','distancetospheresarray','str',1,'','array storing the distances'],
            ['DistanceOffset','offset','float',1,'','offset added to the distances'],
            ['DistanceScale','scale','float',1.,'','scale applied to the distances'],
            ['MinDistance','mindistance','float',1,'','minimum value for the distances'],
            ['MaxDistance','maxdistance','float',1,'','maximum value for the distances'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacities in the scene'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def DistanceParametersValidator(self,text):
        if not text:
            return 1
        splitText = text.strip().split(' ')
        if len(splitText) not in list(range(1,5)):
            return 0
        try:
            for i in range(1,len(splitText)+1):
                float(splitText[i-1])
        except ValueError:
            return 0
        return 1

    def ComputeDistances(self):
        distanceToSpheresFilter = vtkvmtk.vtkvmtkPolyDataDistanceToSpheres()
        distanceToSpheresFilter.SetInputData(self.Surface)
        distanceToSpheresFilter.SetSpheres(self.Spheres)
        distanceToSpheresFilter.SetDistanceOffset(self.DistanceOffset)
        distanceToSpheresFilter.SetDistanceScale(self.DistanceScale)
        distanceToSpheresFilter.SetMinDistance(self.MinDistance);
        distanceToSpheresFilter.SetMaxDistance(self.MaxDistance);
        distanceToSpheresFilter.SetDistanceToSpheresArrayName(self.DistanceToSpheresArrayName)
        distanceToSpheresFilter.Update()
        return distanceToSpheresFilter.GetOutput()

    def InitializeSpheres(self):
        if (self.InteractionMode==0):
            self.Spheres.Initialize()
            seedPoints = vtk.vtkPoints()
            self.Spheres.SetPoints(seedPoints)
            self.Spheres.GetPointData().Initialize()
            seedRadii = vtk.vtkDoubleArray()
            self.Spheres.GetPointData().SetScalars(seedRadii)
            self.CurrentSphereId = -1
            self.SphereWidget.Off()
        else:
            self.ExamineSpheres.Initialize()
            spherePoints = vtk.vtkPoints()
            self.ExamineSpheres.SetPoints(spherePoints)
            self.ExamineSpheres.GetPointData().Initialize()
            sphereRadii = vtk.vtkDoubleArray()
            self.ExamineSpheres.GetPointData().SetScalars(sphereRadii)

    def PlaceSphere(self):
        if self.CurrentSphereId == -1:
            return
        self.SphereWidget.SetCenter(self.Spheres.GetPoint(self.CurrentSphereId))
        self.SphereWidget.SetRadius(self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId))

    def SphereCallback(self,widget,event_string):
        if self.CurrentSphereId == -1:
            return
        minRadius = self.Surface.GetLength()*0.001
        if self.SphereWidget.GetRadius() < minRadius:
            self.SphereWidget.SetRadius(minRadius)
        self.Spheres.GetPoints().SetPoint(self.CurrentSphereId,self.SphereWidget.GetCenter())
        self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,self.SphereWidget.GetRadius())
        self.Spheres.Modified()

    def UndoCallback(self,obj):
        self.InitializeSpheres()
        self.Spheres.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def PickCallback(self,obj):
        picker = vtk.vtkCellPicker()
        picker.SetTolerance(1E-4 * self.Surface.GetLength())
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        #eventPosition = obj.GetEventPosition()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        if result == 0:
            return
        pickPosition = picker.GetPickPosition()
        if (self.InteractionMode==0):
            self.CurrentSphereId = self.Spheres.GetPoints().InsertNextPoint(pickPosition)
            self.Spheres.GetPointData().GetScalars().InsertNextValue(self.Surface.GetLength()*0.01)
            self.Spheres.Modified()
            self.PlaceSphere()
            self.SphereWidget.On()
        else:
            pickedCellPointIds = self.Surface.GetCell(picker.GetCellId()).GetPointIds()
            minDistance = 1E10
            pickedPointId = -1
            for i in range(pickedCellPointIds.GetNumberOfIds()):
                distance = vtk.vtkMath.Distance2BetweenPoints(pickPosition,self.Surface.GetPoint(pickedCellPointIds.GetId(i)))
                if distance < minDistance:
                    minDistance = distance
                    pickedPointId = pickedCellPointIds.GetId(i)
                if pickedPointId == -1:
                    pickedPointId = pickedCellPointIds.GetId(0)
                point = self.Surface.GetPoint(pickedPointId)
                self.ExamineSpheres.GetPoints().InsertNextPoint(point)
                length = 0.
                array = self.ExamineSurface.GetPointData().GetArray(self.DistanceToSpheresArrayName)
                if (array):
                    length = array.GetComponent(pickedPointId,0)
                self.ExamineSpheres.GetPointData().GetScalars().InsertNextValue(length)
                self.ExamineSpheres.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def IncreaseSphereRadiusCallback(self,obj):
        if self.CurrentSphereId != -1:
            #increase sphere radius
            newval = self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId) + self.Surface.GetLength()*0.01
            self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,newval)
            self.Spheres.Modified()
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def DecreaseSphereRadiusCallback(self,obj):
        if self.CurrentSphereId != -1:
            #decrease sphere radius
            newval = self.Spheres.GetPointData().GetScalars().GetValue(self.CurrentSphereId) - self.Surface.GetLength()*0.01
            if newval> 0:
                self.Spheres.GetPointData().GetScalars().SetValue(self.CurrentSphereId,newval)
                self.Spheres.Modified()
                self.PlaceSphere()
                self.vmtkRenderer.RenderWindow.Render()

    def NextCallback(self,obj):
        if self.CurrentSphereId != -1:
            self.CurrentSphereId = (self.CurrentSphereId + 1) % self.Spheres.GetNumberOfPoints();
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def PreviousCallback(self,obj):
        if self.CurrentSphereId != -1:
            self.CurrentSphereId = (self.CurrentSphereId - 1) % self.Spheres.GetNumberOfPoints();
            self.PlaceSphere()
            self.vmtkRenderer.RenderWindow.Render()

    def DistancesCallback(self,obj):
        self.DisplayArray = not self.DisplayArray
        if self.DisplayArray:
            self.ExamineSurface = self.ComputeDistances()
            self.SurfaceMapper.SetInputData(self.ExamineSurface)
            self.ExamineSurface.GetPointData().SetActiveScalars(self.DistanceToSpheresArrayName)
            array = self.ExamineSurface.GetPointData().GetScalars()
            if (array):
                array.Modified()
                self.SurfaceMapper.SetScalarRange(array.GetRange(0))
                self.ScalarBarActor.VisibilityOn()
        else:
            self.SurfaceMapper.SetInputData(self.Surface)
            self.ScalarBarActor.VisibilityOff()
        self.SurfaceMapper.SetScalarVisibility(self.DisplayArray)
        self.vmtkRenderer.RenderWindow.Render()

    def ParametersCallback(self,obj):
        queryString = 'Please input new parameters :\nDistanceOffset('+str(self.DistanceOffset)+') [DistanceScale('+str(self.DistanceScale)+') MinDistance('+str(self.MinDistance)+') MaxDistance('+str(self.MaxDistance)+'): '
        inputString = self.InputText(queryString,self.DistanceParametersValidator)
        splitInputString = inputString.strip().split(' ')
        if len(splitInputString) >= 1 and splitInputString[0] != '':
            self.DistanceOffset = float(splitInputString[0])
        if len(splitInputString) >= 2:
            self.DistanceScale = float(splitInputString[1])
        if len(splitInputString) >= 3:
            self.MinDistance = float(splitInputString[2])
        if len(splitInputString) >= 4:
            self.MaxDistance = float(splitInputString[3])
        self.ParametersChanged = True

    def SwitchModeCallback(self,obj):
        #Switch beetween examien and interact mode
        if self.InteractionMode == 0:
            self.InteractionMode = 1
            self.ExamineSurface = self.ComputeDistances()
            self.SpheresActor.VisibilityOff()
            self.SphereWidget.Off()
            self.ExamineSpheresActor.VisibilityOn()
            self.ExamineText.VisibilityOn()
            self.InitializeSpheres()
        else:
            self.InteractionMode = 0
            #Compute the distances
            self.SpheresActor.VisibilityOn()
            self.ExamineSpheresActor.VisibilityOff()
            self.ExamineText.VisibilityOff()
            if (self.CurrentSphereId!=-1):
                self.SphereWidget.On()
        self.vmtkRenderer.RenderWindow.Render()

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if not self.vmtkRenderer:
          self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
          self.vmtkRenderer.Initialize()
          self.OwnRenderer = 1
          self.vmtkRenderer.RegisterScript(self)
        glyphs = vtk.vtkGlyph3D()
        glyphSource = vtk.vtkSphereSource()
        glyphSource.SetRadius(1)
        glyphs.SetInputData(self.Spheres)
        glyphs.SetSourceConnection(glyphSource.GetOutputPort())
        glyphs.SetScaleModeToScaleByScalar()
        glyphs.SetScaleFactor(1.)
        glyphMapper = vtk.vtkPolyDataMapper()
        glyphMapper.SetInputConnection(glyphs.GetOutputPort())
        glyphMapper.ScalarVisibilityOff()
        self.SpheresActor = vtk.vtkActor()
        self.SpheresActor.SetMapper(glyphMapper)
        self.SpheresActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.SpheresActor.GetProperty().SetOpacity(self.Opacity)
        self.SpheresActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.SpheresActor)

        examineGlyphs = vtk.vtkGlyph3D()
        examineGlyphSource = vtk.vtkSphereSource()
        examineGlyphSource.SetRadius(1)
        examineGlyphs.SetInputData(self.ExamineSpheres)
        examineGlyphs.SetSourceConnection(examineGlyphSource.GetOutputPort())
        examineGlyphs.SetScaleModeToScaleByScalar()
        examineGlyphs.SetScaleFactor(1.)
        examineGlyphMapper = vtk.vtkPolyDataMapper()
        examineGlyphMapper.SetInputConnection(examineGlyphs.GetOutputPort())
        examineGlyphMapper.ScalarVisibilityOff()
        self.ExamineSpheresActor = vtk.vtkActor()
        self.ExamineSpheresActor.SetMapper(examineGlyphMapper)
        self.ExamineSpheresActor.GetProperty().SetColor(0.0,1.0,0.0)
        self.ExamineSpheresActor.GetProperty().SetOpacity(self.Opacity)
        self.ExamineSpheresActor.PickableOff()
        self.ExamineSpheresActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.ExamineSpheresActor)

        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('space','Pick.',self.PickCallback)
        self.vmtkRenderer.AddKeyBinding('+','Increase sphere radius.',self.IncreaseSphereRadiusCallback)
        self.vmtkRenderer.AddKeyBinding('-','Decrease sphere radius.',self.DecreaseSphereRadiusCallback)
        self.vmtkRenderer.AddKeyBinding('n','Skip to next sphere.',self.NextCallback)
        self.vmtkRenderer.AddKeyBinding('v','Skip to previous sphere.',self.PreviousCallback)
        self.vmtkRenderer.AddKeyBinding('d','Show distances graph.',self.DistancesCallback)
        self.vmtkRenderer.AddKeyBinding('a','Change parameters.',self.ParametersCallback)
        self.vmtkRenderer.AddKeyBinding('w','Switch modes.',self.SwitchModeCallback)

        #self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)

        self.SurfaceMapper = vtk.vtkPolyDataMapper()
        self.SurfaceMapper.SetInputData(self.Surface)
        self.SurfaceMapper.SetScalarVisibility(self.DisplayArray)
        surfaceActor = vtk.vtkActor()
        surfaceActor.SetMapper(self.SurfaceMapper)
        surfaceActor.GetProperty().SetOpacity(self.Opacity)
        self.vmtkRenderer.Renderer.AddActor(surfaceActor)

        self.ScalarBarActor = vtk.vtkScalarBarActor()
        self.ScalarBarActor.SetLookupTable(self.SurfaceMapper.GetLookupTable())
        self.ScalarBarActor.GetLabelTextProperty().ItalicOff()
        self.ScalarBarActor.GetLabelTextProperty().BoldOff()
        self.ScalarBarActor.GetLabelTextProperty().ShadowOff()
        self.ScalarBarActor.SetLabelFormat('%.2f')
        self.ScalarBarActor.SetTitle('distances')
        self.ScalarBarActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        self.SphereWidget = vtk.vtkSphereWidget()
        self.SphereWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.SphereWidget.AddObserver("InteractionEvent", self.SphereCallback)

        self.ExamineText = vtk.vtkTextActor()
        self.ExamineText.SetInput("Examine Mode")
        self.ExamineText.GetPositionCoordinate().SetCoordinateSystemToNormalizedViewport()
        self.ExamineText.SetPosition(0.05,0.95)
        self.ExamineText.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor2D(self.ExamineText)

        self.InputInfo('Please position the mouse and press space to add spheres, \'u\' to undo\n')

        any = 0
        while any == 0 or self.ParametersChanged:
            self.ParametersChanged = False
            if any == 0:
                self.InitializeSpheres()
            self.vmtkRenderer.Render()
            any = self.Spheres.GetNumberOfPoints()

        self.Surface = self.ComputeDistances()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
