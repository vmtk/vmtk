#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagelinetracer.py,v $
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

import vtkvmtk
import vmtkrenderer
import pypes

vmtkimagelinetracer = 'vmtkImageLineTracer'

class vmtkImageLineTracer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Line = None

        self.Axis = 2
        self.AutoClose = 0

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.ImageTracerWidget = None

        self.ImageActor = None

        self.SliceVOI = [0,0,0,0,0,0]

        self.Type = 'freehand'
        self.Image = None

        self.SetScriptName('vmtkimagelinetracer')
        self.SetScriptDoc('interactively trace lines on 3D images; press n and p to move to the next and previous slice, respectively')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Axis','axis','int',1,'','id of the drawing plane normal'],
            ['AutoClose','autoclose','bool',1,'','toggle auto close line'],
            ['Type','type','str',1,'["freehand","contour"]','type of widget to use: freehand drawing or control point-based contour']
            ])
        self.SetOutputMembers([
            ['Line','line','vtkPolyData',1,'','the output line','vmtksurfacewriter']
            ])

    def NextCallback(self, obj):
           if self.SliceVOI[self.Axis*2+1] < self.Image.GetWholeExtent()[self.Axis*2+1]:
                self.SliceVOI[self.Axis*2+1] += 1
                self.SliceVOI[self.Axis*2] = self.SliceVOI[self.Axis*2+1]
                origin = self.Image.GetOrigin()
                spacing = self.Image.GetSpacing()
                newOrigin = [origin[0], origin[1], origin[2]]
                newOrigin[self.Axis] = origin[self.Axis]-spacing[self.Axis]
                self.Image.SetOrigin(newOrigin)
                self.ImageActor.SetDisplayExtent(self.SliceVOI)
           obj.Render()

    def PreviousCallback(self, obj):
	   if self.SliceVOI[self.Axis*2] > self.Image.GetWholeExtent()[self.Axis*2]:
                self.SliceVOI[self.Axis*2] -= 1
                self.SliceVOI[self.Axis*2+1] = self.SliceVOI[self.Axis*2]
                origin = self.Image.GetOrigin()
                spacing = self.Image.GetSpacing()
                newOrigin = [origin[0], origin[1], origin[2]]
                newOrigin[self.Axis] = origin[self.Axis]+spacing[self.Axis]
                self.Image.SetOrigin(newOrigin)
                self.ImageActor.SetDisplayExtent(self.SliceVOI)
           obj.Render()

    def InteractCallback(self):
	if self.BoxWidget.GetEnabled() == 1:
	    self.BoxWidget.SetEnabled(0)
	else:
	    self.BoxWidget.SetEnabled(1)

    def SetWidgetProjectionPosition(self,obj,event):
        self.ImageTracerWidget.SetProjectionPosition(self.SliceVOI[self.Axis*2]*self.Image.GetSpacing()[self.Axis]+self.Image.GetOrigin()[self.Axis])
        self.vmtkRenderer.Renderer.Render()

    def GetLineFromWidget(self,obj,event):

        if self.Type == 'freehand':
            path = vtk.vtkPolyData()
            obj.GetPath(path)
        elif self.Type == 'contour':
            path = self.ImageTracerWidget.GetRepresentation().GetContourRepresentationAsPolyData()

        spacing = self.Image.GetSpacing()

        translation = [0.0,0.0,0.0]
        translation[self.Axis] = self.SliceVOI[self.Axis*2]*spacing[self.Axis]

        transform = vtk.vtkTransform()
        transform.Translate(translation)

        pathTransform = vtk.vtkTransformPolyDataFilter()
        pathTransform.SetInput(path)
        pathTransform.SetTransform(transform)
        pathTransform.Update()

        self.Line = pathTransform.GetOutput()

        if self.Line.GetSource():
            self.Line.GetSource().UnRegisterAllOutputs()

    def ChangeSlice(self,obj,event):
        currentSlice = self.SliceVOI[self.Axis*2]
        slice = int(self.SliderWidget.GetRepresentation().GetValue())
        self.SliceVOI[self.Axis*2] = slice
        self.SliceVOI[self.Axis*2+1] = slice
        origin = self.Image.GetOrigin()
        spacing = self.Image.GetSpacing()
        newOrigin = [origin[0], origin[1], origin[2]]
        newOrigin[self.Axis] = origin[self.Axis] - (slice - currentSlice) * spacing[self.Axis]
        self.Image.SetOrigin(newOrigin)
        self.ImageActor.SetDisplayExtent(self.SliceVOI)
        self.vmtkRenderer.Renderer.Render()
 
    def Display(self):

        wholeExtent = self.Image.GetExtent()

        self.SliceVOI[0] = wholeExtent[0]
        self.SliceVOI[1] = wholeExtent[1]
        self.SliceVOI[2] = wholeExtent[2]
        self.SliceVOI[3] = wholeExtent[3]
        self.SliceVOI[4] = wholeExtent[4]
        self.SliceVOI[5] = wholeExtent[5]

        self.SliceVOI[self.Axis*2] = wholeExtent[self.Axis*2]
        self.SliceVOI[self.Axis*2+1] = wholeExtent[self.Axis*2]

        scalarRange = self.Image.GetScalarRange()
        scale = 255.0
        if scalarRange[1]-scalarRange[0] > 0.0:
            scale = 255.0 / (scalarRange[1]-scalarRange[0])

        imageShifter = vtk.vtkImageShiftScale()
        imageShifter.SetInput(self.Image)
        imageShifter.SetShift(-1.0 * scalarRange[0])
        imageShifter.SetScale(scale)
        imageShifter.SetOutputScalarTypeToUnsignedChar()

        widgetImage = imageShifter.GetOutput()

        self.ImageActor.SetInput(widgetImage)
        self.ImageActor.SetDisplayExtent(self.SliceVOI)
        self.vmtkRenderer.Renderer.AddActor(self.ImageActor)

        if self.Type == 'freehand':
            self.ImageTracerWidget.SetCaptureRadius(1.5)
            self.ImageTracerWidget.SetViewProp(self.ImageActor)
            self.ImageTracerWidget.SetInput(widgetImage)
            self.ImageTracerWidget.ProjectToPlaneOn()
            self.ImageTracerWidget.SetProjectionNormal(self.Axis)
            self.ImageTracerWidget.PlaceWidget()
            self.ImageTracerWidget.SetAutoClose(self.AutoClose)
            self.ImageTracerWidget.AddObserver("StartInteractionEvent",self.SetWidgetProjectionPosition)
            self.ImageTracerWidget.AddObserver("EndInteractionEvent",self.GetLineFromWidget)
        elif self.Type == 'contour':
            self.ImageTracerWidget.AddObserver("EndInteractionEvent",self.GetLineFromWidget)
            self.ImageTracerWidget.ContinuousDrawOff()

        sliderRep = vtk.vtkSliderRepresentation2D()
        sliderRep.SetValue(0.5*(wholeExtent[self.Axis*2]+wholeExtent[self.Axis*2+1]))
        sliderRep.SetMinimumValue(wholeExtent[self.Axis*2])
        sliderRep.SetMaximumValue(wholeExtent[self.Axis*2+1])
        sliderRep.SetTitleText("Slice")
        sliderRep.GetPoint1Coordinate().SetCoordinateSystemToNormalizedDisplay()
        sliderRep.GetPoint1Coordinate().SetValue(0.2,0.9)
        sliderRep.GetPoint2Coordinate().SetCoordinateSystemToNormalizedDisplay()
        sliderRep.GetPoint2Coordinate().SetValue(0.8,0.9)
        sliderRep.SetSliderLength(0.02)
        sliderRep.SetSliderWidth(0.03)
        sliderRep.SetEndCapLength(0.01)
        sliderRep.SetEndCapWidth(0.03)
        sliderRep.SetTubeWidth(0.005)
        sliderRep.SetLabelFormat("%.0f")

        self.SliderWidget.AddObserver("InteractionEvent",self.ChangeSlice)
        self.SliderWidget.SetRepresentation(sliderRep)
        self.SliderWidget.EnabledOn()

        interactorStyle = vtk.vtkInteractorStyleImage()
        self.vmtkRenderer.RenderWindowInteractor.SetInteractorStyle(interactorStyle)

        self.vmtkRenderer.Render()

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: no Image.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        if self.Type == 'freehand':
            self.ImageTracerWidget = vtk.vtkImageTracerWidget()
        elif self.Type == 'contour':
            self.ImageTracerWidget = vtk.vtkContourWidget()
        self.ImageTracerWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        self.SliderWidget = vtk.vtkSliderWidget()
        self.SliderWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        self.ImageActor = vtk.vtkImageActor()

        self.vmtkRenderer.AddKeyBinding('n','Next.',self.NextCallback)
        self.vmtkRenderer.AddKeyBinding('p','Previous.',self.PreviousCallback)
        self.vmtkRenderer.AddKeyBinding('i','Interact.', self.InteractCallback)

        self.Display()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
