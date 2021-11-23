#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshviewer2.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## This class is a slightly modified version of vmtkmeshviewer
## which allows for per-cell array visualization, mesh clipping
## and thresholding based on array values.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkMeshViewer2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.Opacity = 1.0
        self.ArrayName = ''
        self.ScalarRange = [0.0, 0.0]
        self.Legend = 0
        self.Grayscale = 0
        self.FlatInterpolation = 0
        self.DisplayCellData = 0
        self.Color = [-1.0, -1.0, -1.0]
        self.Threshold = float('-Inf')
        self.ThresholdUpper = True
        self.DoThreshold = False
        self.ThresholdedMesh = None
        self.InitialMesh = None

        self.PlaneWidget = None
        self.InteractiveClip = False
        self.ClipExtract = False
        self.InsideOut = False
        self.ObserverId = -1

        self.Actor = None
        self.ScalarBarActor = None

        self.SetScriptName('vmtkmeshviewer')
        self.SetScriptDoc('display a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacity in the scene'],
            ['ArrayName','array','str',1,'','name of the array where the scalars to be displayed are stored'],
            ['ScalarRange','scalarrange','float',2,'','range of the scalar map'],
            ['Legend','legend','bool',1,'','toggle scalar bar'],
            ['Grayscale','grayscale','bool',1,'','toggle color or grayscale'],
            ['FlatInterpolation','flat','bool',1,'','toggle flat or shaded surface display'],
            ['DisplayCellData','celldata','bool',1,'','toggle display of point or cell data'],
            ['Color','color','float',3,'','RGB color of the object in the scene'],
            ['Threshold','threshold','float',1,'','threshold to apply to the array when pressing t'],
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def ClipMesh(self):
        meshClipFilter = vtk.vtkClipDataSet()
        meshClipFilter.SetInputData(self.Mesh)
        meshClipFilter.SetInsideOut(self.InsideOut)
        clipPlane = vtk.vtkPlane()
        self.PlaneWidget.GetPlane(clipPlane)
        meshClipFilter.SetClipFunction(clipPlane)
        meshClipFilter.Update()
        return meshClipFilter.GetOutput()

    def ExtractMesh(self):
        meshExtractFilter = vtk.vtkExtractGeometry()
        meshExtractFilter.SetInputData(self.Mesh)
        meshExtractFilter.SetExtractInside(self.InsideOut)
        clipPlane = vtk.vtkPlane()
        self.PlaneWidget.GetPlane(clipPlane)
        meshExtractFilter.SetImplicitFunction(clipPlane)
        meshExtractFilter.Update()
        return meshExtractFilter.GetOutput()

    def ThresholdMesh(self):
        thresholder = vtk.vtkThreshold()
        thresholder.SetInputData(self.InitialMesh)
        if (self.ThresholdUpper):
            thresholder.ThresholdByUpper(self.Threshold)
        else:
            thresholder.ThresholdByLower(self.Threshold)
        thresholder.SetInputArrayToProcess(0,0,0,1,self.ArrayName)
        thresholder.Update()
        self.Mesh = thresholder.GetOutput()

    def PlaneCallback(self,widget,event_string):
        if self.ClipExtract:
            self.Actor.GetMapper().SetInputData(self.ExtractMesh())
        else:
            self.Actor.GetMapper().SetInputData(self.ClipMesh())

    def InteractCallback(self,obj):
        if self.BoxWidget.GetEnabled() == 1:
            self.BoxWidget.SetEnabled(0)
        else:
            self.BoxWidget.SetEnabled(1)

    def ClipCallback(self,obj):
        if self.Mesh != None:
            self.ClipExtract = False
            if self.PlaneWidget.GetEnabled():
                self.Actor.GetMapper().SetInputData(self.ClipMesh())
            else:
                self.Actor.GetMapper().SetInputData(self.Mesh)
            self.vmtkRenderer.RenderWindow.Render()

    def ExtractCallback(self,obj):
        if self.Mesh:
            self.ClipExtract = True
            if self.PlaneWidget.GetEnabled():
                self.Actor.GetMapper().SetInputData(self.ExtractMesh())
            else:
                self.Actor.GetMapper().SetInputData(self.Mesh)
            self.vmtkRenderer.RenderWindow.Render()

    def NCallback(self,obj):
        if self.Actor.GetMapper() != None:
            self.InteractiveClip = not self.InteractiveClip
            if self.InteractiveClip:
                self.ObserverId = self.PlaneWidget.AddObserver("InteractionEvent",self.PlaneCallback)
                self.PlaneWidget.On()
                self.PlaneWidget.InvokeEvent('InteractionEvent')
            elif self.ObserverId != -1:
                self.PlaneWidget.RemoveObserver(self.ObserverId)
                self.ObserverId = -1
                self.PlaneWidget.Off()
                self.Actor.GetMapper().SetInputData(self.Mesh)
            self.vmtkRenderer.RenderWindow.Render()

    def DCallback(self,obj):
        if self.PlaneWidget.GetEnabled():
            self.InsideOut = not self.InsideOut
            self.PlaneWidget.InvokeEvent('InteractionEvent')
            self.vmtkRenderer.RenderWindow.Render()

    def TCallback(self,obj):
        if self.Mesh and self.ArrayName != '':
            #has the mesh been clipped?
            isClipped = (self.Actor.GetMapper().GetInput() != self.Mesh)
            self.DoThreshold = not self.DoThreshold
            if self.DoThreshold:
                self.ThresholdMesh()
            else:
                self.Mesh = self.InitialMesh
            #Redo the clipping if the mesh was clipped
            if isClipped:
                if self.ClipExtract:
                    self.Actor.GetMapper().SetInputData(self.ExtractMesh())
                else:
                    self.Actor.GetMapper().SetInputData(self.ClipMesh())
            else:
                self.Actor.GetMapper().SetInputData(self.Mesh)
            self.vmtkRenderer.RenderWindow.Render()

    def UndoCallback(self,obj):
        self.ThresholdUpper = not self.ThresholdUpper
        if self.DoThreshold:
            isClipped = (self.Actor.GetMapper().GetInput() != self.Mesh)
            self.ThresholdMesh()
            if isClipped:
                if self.ClipExtract:
                    self.Actor.GetMapper().SetInputData(self.ExtractMesh())
                else:
                    self.Actor.GetMapper().SetInputData(self.ClipMesh())
            else:
                self.Actor.GetMapper().SetInputData(self.Mesh)
            self.vmtkRenderer.RenderWindow.Render()

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        if self.Actor != None:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.ScalarBarActor != None:
            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.Mesh != None:
            self.InitialMesh = self.Mesh
            self.DisplayMesh = self.Mesh
            mapper = vtk.vtkDataSetMapper()
            mapper.SetInputData(self.DisplayMesh)
            array = None
            if (self.ArrayName != ''):
                if self.DisplayCellData == 0:
                  self.Mesh.GetPointData().SetActiveScalars(self.ArrayName)
                  array = self.Mesh.GetPointData().GetScalars()
                else:
                  self.Mesh.GetCellData().SetActiveScalars(self.ArrayName)
                  array = self.Mesh.GetCellData().GetScalars()
                  mapper.SetScalarModeToUseCellData()
            if (array != None):
                if (self.ScalarRange[1] > self.ScalarRange[0]):
                    mapper.SetScalarRange(self.ScalarRange)
                else:
                    mapper.SetScalarRange(array.GetRange(0))
                if (self.Grayscale == 1):
                    lut = vtk.vtkLookupTable()
                    lut.SetValueRange(0.0,1.0)
                    lut.SetSaturationRange(0.0,0.0)
                    mapper.SetLookupTable(lut)

            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            if (self.Color[0] >= 0.0):
                self.Actor.GetProperty().SetColor(self.Color)
            if (self.FlatInterpolation == 1):
                self.Actor.GetProperty().SetInterpolationToFlat()
            self.Actor.GetProperty().SetOpacity(self.Opacity)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)

        if (self.Legend == 1) & (self.Actor != None):
            self.ScalarBarActor = vtk.vtkScalarBarActor()
            self.ScalarBarActor.SetLookupTable(self.Actor.GetMapper().GetLookupTable())
            self.ScalarBarActor.GetLabelTextProperty().ItalicOff()
            self.ScalarBarActor.GetLabelTextProperty().BoldOff()
            self.ScalarBarActor.GetLabelTextProperty().ShadowOff()
##             self.ScalarBarActor.GetLabelTextProperty().SetColor(0.0,0.0,0.0)
            self.ScalarBarActor.SetLabelFormat('%.2f')
            self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        #self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)
        self.vmtkRenderer.AddKeyBinding('i','Interact.',self.InteractCallback)
        self.vmtkRenderer.AddKeyBinding('c','Clip.',self.ClipCallback)
        self.vmtkRenderer.AddKeyBinding('e','Extract.',self.ExtractCallback)
        self.vmtkRenderer.AddKeyBinding('n','Show clipped area.',self.NCallback)
        self.vmtkRenderer.AddKeyBinding('d','Switch clipped/unclipped area.',self.DCallback)
        self.vmtkRenderer.AddKeyBinding('t','Redo.',self.TCallback)
        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.PlaneWidget = vtk.vtkImplicitPlaneWidget()
        self.PlaneWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidget.SetPlaceFactor(1.25)
        self.PlaneWidget.DrawPlaneOff()
        self.PlaneWidget.SetProp3D(self.Actor)
        self.PlaneWidget.PlaceWidget()
        #Work around bug/strange behaviour in vtk
        self.PlaneWidget.SetOrigin(self.Actor.GetCenter())

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        self.Mesh = self.InitialMesh

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Mesh == None) & (self.Display == 1):
            self.PrintError('Error: no Mesh.')

        self.BuildView()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
