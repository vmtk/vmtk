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

import vtkvmtk
import vmtkrenderer
import vtk
import sys

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

        self.Image = None

        self.SetScriptName('vmtkimagelinetracer')
        self.SetScriptDoc('interactively trace lines on 3D images; press n and p to move to the next and previous slice, respectively')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'the input image','vmtkimagereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'external renderer'],
            ['Axis','axis','int',1,'id of the drawing plane normal'],
            ['AutoClose','autoclose','int',1,'toggle auto close line']
            ])
        self.SetOutputMembers([
            ['Line','line','vtkPolyData',1,'the output line','vmtksurfacewriter']
            ])

    def Keypress(self,obj,event):
        key = obj.GetKeySym()
        if key == 'n':
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
        elif key == 'p':
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

    def SetWidgetProjectionPosition(self,obj,event):
        self.ImageTracerWidget.SetProjectionPosition(self.SliceVOI[self.Axis*2]*self.Image.GetSpacing()[self.Axis]+self.Image.GetOrigin()[self.Axis])
        self.vmtkRenderer.Renderer.Render()

    def GetLineFromWidget(self,obj,event):

        path = vtk.vtkPolyData()
        obj.GetPath(path)

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

    def Display(self):

        wholeExtent = self.Image.GetWholeExtent()

        self.SliceVOI[0] = wholeExtent[0]
        self.SliceVOI[1] = wholeExtent[1]
        self.SliceVOI[2] = wholeExtent[2]
        self.SliceVOI[3] = wholeExtent[3]
        self.SliceVOI[4] = wholeExtent[4]
        self.SliceVOI[5] = wholeExtent[5]

        self.SliceVOI[self.Axis*2] = wholeExtent[self.Axis*2]
        self.SliceVOI[self.Axis*2+1] = wholeExtent[self.Axis*2]

        range = self.Image.GetScalarRange()

        imageShifter = vtk.vtkImageShiftScale()
        imageShifter.SetInput(self.Image)
        imageShifter.SetShift(-1.0*range[0])
        imageShifter.SetScale(255.0/(range[1]-range[0]))
        imageShifter.SetOutputScalarTypeToUnsignedChar()

        widgetImage = imageShifter.GetOutput()

        self.ImageActor.SetInput(widgetImage)
        self.ImageActor.SetDisplayExtent(self.SliceVOI)
        self.vmtkRenderer.Renderer.AddActor(self.ImageActor)

        self.ImageTracerWidget.SetCaptureRadius(1.5)
        self.ImageTracerWidget.SetProp(self.ImageActor)
        self.ImageTracerWidget.SetInput(widgetImage)
        self.ImageTracerWidget.ProjectToPlaneOn()
        self.ImageTracerWidget.SetProjectionNormal(self.Axis)
        self.ImageTracerWidget.PlaceWidget()
        self.ImageTracerWidget.SetAutoClose(self.AutoClose)
        self.ImageTracerWidget.AddObserver("StartInteractionEvent",self.SetWidgetProjectionPosition)
        self.ImageTracerWidget.AddObserver("EndInteractionEvent",self.GetLineFromWidget)

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

        self.ImageTracerWidget = vtk.vtkImageTracerWidget()
        self.ImageTracerWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        self.ImageActor = vtk.vtkImageActor()

        self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.Keypress)

        self.Display()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
