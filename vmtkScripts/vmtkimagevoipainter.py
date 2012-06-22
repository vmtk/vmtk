#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevoipainter.py,v $
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
import math

import vtkvmtk
import vmtkrenderer
import pypes

vmtkimagevoipainter = 'vmtkImageVOIPainter'

class vmtkImageVOIPainter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.CubeSource = vtk.vtkCubeSource()
        self.CubeActor = vtk.vtkActor()
        
        self.BoxActive = 0
        self.BoxBounds = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.PaintValue = 0.0

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None
        self.BoxWidget = None

        self.Image = None

        self.Interactive = 1

        self.SetScriptName('vmtkimagevoipainter')
        self.SetScriptDoc('fill a cubical region of an image with a given gray level')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Interactive','interactive','bool',1,'','toggle interactivity'],
            ['BoxBounds','boxbounds','float',6,'','bounds of the cubical region for non-interactive mode'],
            ['PaintValue','paintvalue','float',1,'','graylevel to fill the region with'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def InteractCallback(self):
        if self.BoxWidget.GetEnabled() == 1:
            self.BoxWidget.SetEnabled(0)
        else:
            self.BoxWidget.SetEnabled(1)

    def HideCube(self,object, event):
        self.CubeActor.VisibilityOff()

    def UpdateCube(self,object, event):
        polyData = vtk.vtkPolyData()
        object.GetPolyData(polyData)
        polyData.ComputeBounds()
        self.CubeSource.SetBounds(polyData.GetBounds())
        self.CubeSource.Modified()
        self.CubeActor.VisibilityOn()

    def Display(self):

        wholeExtent = self.Image.GetWholeExtent()

        picker = vtk.vtkCellPicker()
        picker.SetTolerance(0.005)

        self.PlaneWidgetX.SetInput(self.Image)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        self.PlaneWidgetX.DisplayTextOn()
        self.PlaneWidgetX.SetPicker(picker)
        self.PlaneWidgetX.KeyPressActivationOff()
        self.PlaneWidgetX.On()

        self.PlaneWidgetY.SetInput(self.Image)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        self.PlaneWidgetY.DisplayTextOn()
        self.PlaneWidgetY.SetPicker(picker)
        self.PlaneWidgetY.KeyPressActivationOff()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetY.On()

        self.PlaneWidgetZ.SetInput(self.Image)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        self.PlaneWidgetZ.DisplayTextOn()
        self.PlaneWidgetZ.SetPicker(picker)
        self.PlaneWidgetZ.KeyPressActivationOff()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetZ.On()

        self.BoxWidget.SetPriority(1.0)
        self.BoxWidget.SetHandleSize(5E-3)
        self.BoxWidget.SetInput(self.Image)
        self.BoxWidget.PlaceWidget()
        self.BoxWidget.RotationEnabledOff()
        self.BoxWidget.AddObserver("StartInteractionEvent", self.HideCube)
        self.BoxWidget.AddObserver("EndInteractionEvent", self.UpdateCube)
        self.BoxWidget.AddObserver("EnableEvent", self.UpdateCube)
        self.BoxWidget.AddObserver("DisableEvent", self.HideCube)

        polyData = vtk.vtkPolyData()
        self.BoxWidget.GetPolyData(polyData)
        polyData.ComputeBounds()
        self.CubeSource.SetBounds(polyData.GetBounds())
        cubeMapper = vtk.vtkPolyDataMapper()
        cubeMapper.SetInput(self.CubeSource.GetOutput())
        self.CubeActor.SetMapper(cubeMapper)
        self.CubeActor.GetProperty().SetColor(0.6,0.6,0.2)
        self.CubeActor.GetProperty().SetOpacity(0.25)
        self.CubeActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.CubeActor)

        self.vmtkRenderer.Render()

        self.vmtkRenderer.Renderer.RemoveActor(self.CubeActor)

        self.BoxActive = 0
        if self.BoxWidget.GetEnabled() == 1:
            polyData = vtk.vtkPolyData()
            self.BoxWidget.GetPolyData(polyData)
            polyData.ComputeBounds()
            bounds = polyData.GetBounds()
            self.BoxBounds[0] = bounds[0]
            self.BoxBounds[1] = bounds[1]
            self.BoxBounds[2] = bounds[2]
            self.BoxBounds[3] = bounds[3]
            self.BoxBounds[4] = bounds[4]
            self.BoxBounds[5] = bounds[5]
            self.BoxActive = 1

        self.BoxWidget.Off()

    def PaintVOI(self):

        wholeExtent = self.Image.GetWholeExtent()[:]
        origin = self.Image.GetOrigin()
        spacing = self.Image.GetSpacing()

        paintedVOI = [0,0,0,0,0,0]
        paintedVOI[0] = max(wholeExtent[0],int(math.ceil((self.BoxBounds[0]-origin[0])/spacing[0])))
        paintedVOI[1] = min(wholeExtent[1],int(math.floor((self.BoxBounds[1]-origin[0])/spacing[0])))
        paintedVOI[2] = max(wholeExtent[2],int(math.ceil((self.BoxBounds[2]-origin[1])/spacing[1])))
        paintedVOI[3] = min(wholeExtent[3],int(math.floor((self.BoxBounds[3]-origin[1])/spacing[1])))
        paintedVOI[4] = max(wholeExtent[4],int(math.ceil((self.BoxBounds[4]-origin[2])/spacing[2])))
        paintedVOI[5] = min(wholeExtent[5],int(math.floor((self.BoxBounds[5]-origin[2])/spacing[2])))

        # extent trick. TODO: fix vtkvmtkImageBoxPainter
        paintedVOI[0] -= wholeExtent[0]
        paintedVOI[1] -= wholeExtent[0]
        paintedVOI[2] -= wholeExtent[2]
        paintedVOI[3] -= wholeExtent[2]
        paintedVOI[4] -= wholeExtent[4]
        paintedVOI[5] -= wholeExtent[4]

        translate = vtk.vtkImageTranslateExtent()
        translate.SetInput(self.Image)
        translate.SetTranslation(-wholeExtent[0],-wholeExtent[2],-wholeExtent[4])
        translate.Update()

        imageBoxPainter = vtkvmtk.vtkvmtkImageBoxPainter()
        imageBoxPainter.SetInput(translate.GetOutput())
        imageBoxPainter.SetBoxExtent(paintedVOI)
        imageBoxPainter.SetBoxDefinitionToUseExtent()
        imageBoxPainter.SetPaintValue(self.PaintValue)
        imageBoxPainter.Update()

        translate = vtk.vtkImageTranslateExtent()
        translate.SetInput(imageBoxPainter.GetOutput())
        translate.SetTranslation(wholeExtent[0],wholeExtent[2],wholeExtent[4])
        translate.Update()

        self.Image.ShallowCopy(translate.GetOutput())

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: no Image.')

        if self.Interactive == 1:
            
            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)                 

            self.PlaneWidgetX = vtk.vtkImagePlaneWidget()
            self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.PlaneWidgetY = vtk.vtkImagePlaneWidget()
            self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.PlaneWidgetZ = vtk.vtkImagePlaneWidget()
            self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.BoxWidget = vtk.vtkBoxWidget()
            self.BoxWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.vmtkRenderer.AddKeyBinding('i','Interact.', self.InteractCallback)

            self.Display()
            while (self.BoxActive == 1):
                self.PaintVOI()
                self.Display()
        else:
            self.PaintVOI()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
