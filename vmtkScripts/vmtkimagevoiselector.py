#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevoiselector.py,v $
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
import math

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkImageVOISelector(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.CubeSource = vtk.vtkCubeSource()
        self.CubeActor = vtk.vtkActor()

        self.BoxActive = 0
        self.BoxBounds = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

        self.CroppedImage = vtk.vtkImageData()

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None
        self.BoxWidget = None

        self.Image = None

        self.Interactive = 1

        self.SetScriptName('vmtkimagevoiselector')
        self.SetScriptDoc('select a cubical volume of interest and get rid of the rest of the image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['Interactive','interactive','bool',1,'','toggle interactivity'],
            ['BoxBounds','boxbounds','float',6,'','bounds of the cubical region of interest'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    #def InteractCallback(self, obj):
    #    if self.BoxWidget.GetEnabled() == 1:
    #        self.BoxWidget.SetEnabled(0)
    #    else:
    #        self.BoxWidget.SetEnabled(1)

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

        wholeExtent = self.Image.GetExtent()

        picker = vtk.vtkCellPicker()
        picker.SetTolerance(0.005)

        self.PlaneWidgetX.SetInputData(self.Image)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetX.DisplayTextOn()
        else:
            self.PlaneWidgetX.DisplayTextOff()
        self.PlaneWidgetX.SetPicker(picker)
        self.PlaneWidgetX.KeyPressActivationOff()
        self.PlaneWidgetX.On()

        self.PlaneWidgetY.SetInputData(self.Image)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetY.DisplayTextOn()
        else:
            self.PlaneWidgetY.DisplayTextOff()
        self.PlaneWidgetY.SetPicker(picker)
        self.PlaneWidgetY.KeyPressActivationOff()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetY.On()

        self.PlaneWidgetZ.SetInputData(self.Image)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetZ.DisplayTextOn()
        else:
            self.PlaneWidgetZ.DisplayTextOff()
        self.PlaneWidgetZ.SetPicker(picker)
        self.PlaneWidgetZ.KeyPressActivationOff()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetZ.On()

        self.BoxWidget.SetPriority(1.0)
        self.BoxWidget.SetHandleSize(5E-3)
        self.BoxWidget.SetInputData(self.Image)
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
        cubeMapper.SetInputConnection(self.CubeSource.GetOutputPort())
        self.CubeActor.SetMapper(cubeMapper)
        self.CubeActor.GetProperty().SetColor(0.6,0.6,0.2)
        self.CubeActor.GetProperty().SetOpacity(0.25)
        self.CubeActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.CubeActor)

        self.vmtkRenderer.Renderer.ResetCamera()

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

    def ExtractVOI(self):

        wholeExtent = self.Image.GetExtent()
        origin = self.Image.GetOrigin()
        spacing = self.Image.GetSpacing()

        newVOI = [0,0,0,0,0,0]
        newVOI[0] = max(wholeExtent[0],int(math.ceil((self.BoxBounds[0]-origin[0])/spacing[0])))
        newVOI[1] = min(wholeExtent[1],int(math.floor((self.BoxBounds[1]-origin[0])/spacing[0])))
        newVOI[2] = max(wholeExtent[2],int(math.ceil((self.BoxBounds[2]-origin[1])/spacing[1])))
        newVOI[3] = min(wholeExtent[3],int(math.floor((self.BoxBounds[3]-origin[1])/spacing[1])))
        newVOI[4] = max(wholeExtent[4],int(math.ceil((self.BoxBounds[4]-origin[2])/spacing[2])))
        newVOI[5] = min(wholeExtent[5],int(math.floor((self.BoxBounds[5]-origin[2])/spacing[2])))

        extractVOI = vtk.vtkExtractVOI()
        extractVOI.SetInputData(self.CroppedImage)
        extractVOI.SetVOI(newVOI)
        extractVOI.Update()

        self.CroppedImage.DeepCopy(extractVOI.GetOutput())

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: no Image.')

        self.CroppedImage.DeepCopy(self.Image)

        if self.Interactive == 1:

            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)
            self.PlaneWidgetX = vtkvmtk.vtkvmtkImagePlaneWidget()
            self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.PlaneWidgetY = vtkvmtk.vtkvmtkImagePlaneWidget()
            self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.PlaneWidgetZ = vtkvmtk.vtkvmtkImagePlaneWidget()
            self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.BoxWidget = vtk.vtkBoxWidget()
            self.BoxWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

            #self.vmtkRenderer.AddKeyBinding('i','Interact.',self.InteractCallback)
            self.InputInfo("Press 'i' to activate interactor")

            self.Display()
            while (self.BoxActive == 1):
                self.ExtractVOI()
                self.Image = self.CroppedImage
                self.Display()
        else:
            self.ExtractVOI()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

        self.Image = self.CroppedImage


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
