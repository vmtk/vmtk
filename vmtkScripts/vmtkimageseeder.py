#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageseeder.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkImageSeeder(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''
        self.SeedRGBColor = [1.0, 0.0, 0.0] # red by default

        self.Picker = None
        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None

        self.SeedActor = None

        self.Seeds = None

        self.TextureInterpolation = 1
        self.KeepSeeds = 0

        self.SetScriptName('vmtkimageseeder')
        self.SetScriptDoc('interactively place seeds in a 3D image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['KeepSeeds','keepseeds','bool',1,'','toggle avoid removal of seeds from renderer'],
            ['SeedRGBColor','seedcolor','float',3,'(0.0,1.0)','RGB Values to set the color of the seed to. red by default'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['Seeds','seeds','vtkPolyData',1,'','the placed seeds','vmtksurfacewriter']
            ])

    def AddSeed(self, obj, event):
        if self.vmtkRenderer.RenderWindowInteractor.GetControlKey() == 0:
            return
        cursorData = [0.0,0.0,0.0,0.0]
        obj.GetCursorData(cursorData)
        spacing = self.Image.GetSpacing()
        origin = self.Image.GetOrigin()
        extent = self.Image.GetExtent()
        point = [0.0,0.0,0.0]
        point[0] = cursorData[0] * spacing[0] + origin[0]
        point[1] = cursorData[1] * spacing[1] + origin[1]
        point[2] = cursorData[2] * spacing[2] + origin[2]
        self.Seeds.GetPoints().InsertNextPoint(point)
        self.Seeds.Modified()
        self.vmtkRenderer.RenderWindow.Render()

    def WidgetsOn(self):
        self.PlaneWidgetX.On()
        self.PlaneWidgetY.On()
        self.PlaneWidgetZ.On()

    def WidgetsOff(self):
        self.PlaneWidgetX.Off()
        self.PlaneWidgetY.Off()
        self.PlaneWidgetZ.Off()

    def InitializeSeeds(self):
        self.Seeds.Initialize()
        seedPoints = vtk.vtkPoints()
        self.Seeds.SetPoints(seedPoints)

    def BuildView(self):

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)
        wholeExtent = self.Image.GetExtent()

#        self.PlaneWidgetX.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetX.SetResliceInterpolateToLinear()
        self.PlaneWidgetX.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetX.SetInputData(self.Image)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetX.DisplayTextOn()
        else:
            self.PlaneWidgetX.DisplayTextOff()
        self.PlaneWidgetX.KeyPressActivationOff()

#        self.PlaneWidgetY.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetY.SetResliceInterpolateToLinear()
        self.PlaneWidgetY.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetY.SetInputData(self.Image)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetY.DisplayTextOn()
        else:
            self.PlaneWidgetY.DisplayTextOff()
        self.PlaneWidgetY.KeyPressActivationOff()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())

#        self.PlaneWidgetZ.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetZ.SetResliceInterpolateToLinear()
        self.PlaneWidgetZ.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetZ.SetInputData(self.Image)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetZ.DisplayTextOn()
        else:
            self.PlaneWidgetZ.DisplayTextOff()
        self.PlaneWidgetZ.KeyPressActivationOff()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())

        glyphs = vtk.vtkGlyph3D()
        glyphSource = vtk.vtkSphereSource()
        glyphs.SetInputData(self.Seeds)
        glyphs.SetSourceConnection(glyphSource.GetOutputPort())
        glyphs.SetScaleModeToDataScalingOff()
        glyphs.SetScaleFactor(self.Image.GetLength()*0.01)
        glyphMapper = vtk.vtkPolyDataMapper()
        glyphMapper.SetInputConnection(glyphs.GetOutputPort())
        self.SeedActor = vtk.vtkActor()
        self.SeedActor.SetMapper(glyphMapper)
        self.SeedActor.GetProperty().SetColor(self.SeedRGBColor[0],
                                              self.SeedRGBColor[1],
                                              self.SeedRGBColor[2])
        self.vmtkRenderer.Renderer.AddActor(self.SeedActor)

        self.WidgetsOn()

        if (self.Display == 1):
            self.vmtkRenderer.AddKeyBinding('Ctrl','Add Seed.')

            self.vmtkRenderer.Render()

    def Execute(self):
        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        ##self.PrintLog('Ctrl +  left click to add seed.')
        self.Picker = vtk.vtkCellPicker()
        self.Picker.SetTolerance(0.005)

        self.PlaneWidgetX = vtkvmtk.vtkvmtkImagePlaneWidget()
        self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetX.AddObserver("StartInteractionEvent", self.AddSeed)
        self.PlaneWidgetX.SetPicker(self.Picker)
        self.PlaneWidgetY = vtkvmtk.vtkvmtkImagePlaneWidget()
        self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetY.AddObserver("StartInteractionEvent", self.AddSeed)
        self.PlaneWidgetY.SetPicker(self.Picker)
        self.PlaneWidgetZ = vtkvmtk.vtkvmtkImagePlaneWidget()
        self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetZ.AddObserver("StartInteractionEvent", self.AddSeed)
        self.PlaneWidgetZ.SetPicker(self.Picker)

        self.Seeds = vtk.vtkPolyData()
        self.InitializeSeeds()

        self.BuildView()

        self.WidgetsOff()

        if not self.KeepSeeds:
            self.vmtkRenderer.Renderer.RemoveActor(self.SeedActor)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
