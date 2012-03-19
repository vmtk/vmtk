#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import sys

import vmtkrenderer
import pypes

vmtksurfaceclipper = 'vmtkSurfaceClipper'

class vmtkSurfaceClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.WidgetType = 'box'

        self.Actor = None
        self.ClipWidget = None
        self.ClipFunction = None
        self.CleanOutput = 1
        self.Transform = None

        self.SetScriptName('vmtksurfaceclipper')
        self.SetScriptDoc('interactively clip a surface with a box')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['WidgetType','type','str',1,'["box","sphere"]','type of widget used for clipping'],
            ['Transform','transform','vtkTransform',1,'','the widget transform, useful in case of piping of multiple clipping scripts'],
            ['CleanOutput','cleanoutput','bool',1,'','toggle cleaning the unused points'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
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
        self.Surface.DeepCopy(self.Clipper.GetClippedOutput())
        self.Surface.Update()
        self.ClipWidget.Off()

    def Display(self):

      	self.ClipWidget.SetInput(self.Surface)
      	self.ClipWidget.PlaceWidget()

        if self.Transform:
            self.ClipWidget.SetTransform(self.Transform)
            self.ClipWidget.On()

      	self.vmtkRenderer.RenderWindowInteractor.Initialize()

        self.vmtkRenderer.Render()
        self.vmtkRenderer.RenderWindowInteractor.Start()

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if self.WidgetType == "box":
            self.ClipFunction = vtk.vtkPlanes()
        elif self.WidgetType == "sphere":
            self.ClipFunction = vtk.vtkSphere()

        self.Clipper = vtk.vtkClipPolyData()
        self.Clipper.SetInput(self.Surface)
        self.Clipper.SetClipFunction(self.ClipFunction)
        self.Clipper.GenerateClippedOutputOn()
        self.Clipper.InsideOutOn()
        
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInput(self.Surface)
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
        self.vmtkRenderer.AddKeyBinding('i','Interact.')
        self.Display()

        self.Transform = vtk.vtkTransform()
        self.ClipWidget.GetTransform(self.Transform)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

        if self.CleanOutput == 1:
            cleaner = vtk.vtkCleanPolyData()
            cleaner.SetInput(self.Surface)
            cleaner.Update()
            self.Surface = cleaner.GetOutput()

        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
