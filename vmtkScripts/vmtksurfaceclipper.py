#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkSurfaceClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ClippedSurface = None
        self.CutLines = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.WidgetType = 'box'

        self.Actor = None
        self.ClipWidget = None
        self.ClipFunction = None
        self.CleanOutput = 1
        self.Transform = None

        self.InsideOut = 0

        self.Interactive = 1
        self.ClipArrayName = None
        self.ClipValue = 0.0

        self.SetScriptName('vmtksurfaceclipper')
        self.SetScriptDoc('interactively clip a surface with a box')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['WidgetType','type','str',1,'["box","sphere"]','type of widget used for clipping'],
            ['Transform','transform','vtkTransform',1,'','the widget transform, useful in case of piping of multiple clipping scripts'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['InsideOut','insideout','bool',1,'','toggle switching output and clipped surfaces'],
            ['Interactive','interactive','bool',1,'','toggle clipping with a widget or with a pre-defined scalar field'],
            ['ClipArrayName','array','str',1,'','name of the array with which to clip in case of non-interactive clipping'],
            ['ClipValue','value','float',1,'','scalar value at which to perform clipping in case of non-interactive clipping'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['ClippedSurface','oclipped','vtkPolyData',1,'','the clipped surface','vmtksurfacewriter'],
            ['CutLines','ocutlines','vtkPolyData',1,'','the cutlines','vmtksurfacewriter'],
            ['Transform','otransform','vtkTransform',1,'','the output widget transform']
            ])

    def ClipCallback(self, obj):
        if self.ClipWidget.GetEnabled() != 1:
            return
        if self.WidgetType == "box":
            self.ClipWidget.GetPlanes(self.ClipFunction)
        elif self.WidgetType == "sphere":
            self.ClipWidget.GetSphere(self.ClipFunction)
        self.Clipper.Update()
        self.Surface.DeepCopy(self.Clipper.GetOutput())
        self.ClippedSurface.DeepCopy(self.Clipper.GetClippedOutput())
        self.Cutter.Update()
        self.CutLines.DeepCopy(self.Cutter.GetOutput())
        self.ClipWidget.Off()

    def InteractCallback(self, obj):
        pass
    #    if self.ClipWidget.GetEnabled() == 1:
    #        self.ClipWidget.SetEnabled(0)
    #    else:
    #        self.ClipWidget.SetEnabled(1)

    def Display(self):

        self.ClipWidget.SetInputData(self.Surface)
        self.ClipWidget.PlaceWidget()

        if self.Transform and self.WidgetType == "box":
            self.ClipWidget.SetTransform(self.Transform)
            self.ClipWidget.On()

        #self.vmtkRenderer.RenderWindowInteractor.Initialize()
        self.vmtkRenderer.Render()
        #self.vmtkRenderer.RenderWindowInteractor.Start()

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.Clipper = vtk.vtkClipPolyData()
        self.Clipper.SetInputData(self.Surface)
        self.Clipper.GenerateClippedOutputOn()
        self.Clipper.SetInsideOut(self.InsideOut)

        if self.Interactive:

            if self.WidgetType == "box":
                self.ClipFunction = vtk.vtkPlanes()
            elif self.WidgetType == "sphere":
                self.ClipFunction = vtk.vtkSphere()

            self.Clipper.SetClipFunction(self.ClipFunction)

            self.Cutter = vtk.vtkCutter()
            self.Cutter.SetInputData(self.Surface)
            self.Cutter.SetCutFunction(self.ClipFunction)

            self.ClippedSurface = vtk.vtkPolyData()
            self.CutLines = vtk.vtkPolyData()

            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInputData(self.Surface)
            mapper.ScalarVisibilityOff()
            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)

            if self.WidgetType == "box":
                self.ClipWidget = vtk.vtkBoxWidget()
                self.ClipWidget.GetFaceProperty().SetColor(0.6,0.6,0.2)
                self.ClipWidget.GetFaceProperty().SetOpacity(0.25)
            elif self.WidgetType == "sphere":
                self.ClipWidget = vtk.vtkSphereWidget()
                self.ClipWidget.GetSphereProperty().SetColor(0.6,0.6,0.2)
                self.ClipWidget.GetSphereProperty().SetOpacity(0.25)
                self.ClipWidget.GetSelectedSphereProperty().SetColor(0.6,0.0,0.0)
                self.ClipWidget.GetSelectedSphereProperty().SetOpacity(0.75)
                self.ClipWidget.SetRepresentationToSurface()
                self.ClipWidget.SetPhiResolution(20)
                self.ClipWidget.SetThetaResolution(20)

            self.ClipWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

            self.vmtkRenderer.AddKeyBinding('space','Clip.',self.ClipCallback)
            self.vmtkRenderer.AddKeyBinding('i','Interact.',self.InteractCallback)
            self.Display()

            self.Transform = vtk.vtkTransform()
            if self.WidgetType == "box":
                self.ClipWidget.GetTransform(self.Transform)

            if self.OwnRenderer:
                self.vmtkRenderer.Deallocate()

        else:

            self.Surface.GetPointData().SetActiveScalars(self.ClipArrayName)

            self.Clipper.GenerateClipScalarsOff()
            self.Clipper.SetValue(self.ClipValue)
            self.Clipper.Update()

            self.Cutter = vtk.vtkContourFilter()
            self.Cutter.SetInputData(self.Surface)
            self.Cutter.SetValue(0,self.ClipValue)
            self.Cutter.Update()

            self.Surface = self.Clipper.GetOutput()
            self.ClippedSurface = self.Clipper.GetClippedOutput()

            self.CutLines = self.Cutter.GetOutput()

        if self.CleanOutput == 1:

            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputData(self.Surface)
            cleaner.Update()
            self.Surface = cleaner.GetOutput()

            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputData(self.ClippedSurface)
            cleaner.Update()
            self.ClippedSurface = cleaner.GetOutput()

            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInputData(self.CutLines)
            cleaner.Update()
            stripper = vtk.vtkStripper()
            stripper.SetInputConnection(cleaner.GetOutputPort())
            stripper.Update()
            self.CutLines = stripper.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
