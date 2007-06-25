#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.8 $

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

vmtkimageviewer = 'vmtkImageViewer'

class vmtkImageViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.Picker = None
        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None

        self.TextureInterpolation = 0

        self.SetScriptName('vmtkimageviewer')
        self.SetScriptDoc('display a 3D image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)
        wholeExtent = self.Image.GetWholeExtent()

        if self.Picker == None:
            self.Picker = vtk.vtkCellPicker()

        if self.PlaneWidgetX == None:
            self.PlaneWidgetX = vtk.vtkImagePlaneWidget()

        if self.PlaneWidgetY == None:
            self.PlaneWidgetY = vtk.vtkImagePlaneWidget()

        if self.PlaneWidgetZ == None:
            self.PlaneWidgetZ = vtk.vtkImagePlaneWidget()
            
        self.Picker.SetTolerance(0.005)

        self.PlaneWidgetX.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetX.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetX.SetInput(self.Image)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        self.PlaneWidgetX.DisplayTextOn()
        self.PlaneWidgetX.SetPicker(self.Picker)
        self.PlaneWidgetX.KeyPressActivationOff()
        self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetX.On()

        self.PlaneWidgetY.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetY.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetY.SetInput(self.Image)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        self.PlaneWidgetY.DisplayTextOn()
        self.PlaneWidgetY.SetPicker(self.Picker)
        self.PlaneWidgetY.KeyPressActivationOff()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetY.On()

        self.PlaneWidgetZ.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetZ.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetZ.SetInput(self.Image)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        self.PlaneWidgetZ.DisplayTextOn()
        self.PlaneWidgetZ.SetPicker(self.Picker)
        self.PlaneWidgetZ.KeyPressActivationOff()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetZ.On()

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')
 
        self.BuildView()
       
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
