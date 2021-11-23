#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageviewer.py,v $
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

        self.Margins = 0
        self.TextureInterpolation = 1
        self.ContinuousCursor = 0
        self.WindowLevel = [0.0, 0.0]

        self.SetScriptName('vmtkimageviewer')
        self.SetScriptDoc('display a 3D image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['WindowLevel','windowlevel','float',2,'','the window/level for displaying the image'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Margins','margins','bool',1,'','toggle margins for tilting image planes'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes'],
            ['ContinuousCursor','continuouscursor','bool',1,'','toggle use of physical continuous coordinates for the cursor']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['PlaneWidgetX','xplane','vtkImagePlaneWidget',1,'','the X image plane widget'],
            ['PlaneWidgetY','yplane','vtkImagePlaneWidget',1,'','the Y image plane widget'],
            ['PlaneWidgetZ','zplane','vtkImagePlaneWidget',1,'','the Z image plane widget']
            ])

    def CharCallback(self, obj):
        return

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)
        wholeExtent = self.Image.GetExtent()

        if self.Picker == None:
            self.Picker = vtk.vtkCellPicker()

        if self.PlaneWidgetX == None:
            self.PlaneWidgetX = vtkvmtk.vtkvmtkImagePlaneWidget()

        if self.PlaneWidgetY == None:
            self.PlaneWidgetY = vtkvmtk.vtkvmtkImagePlaneWidget()

        if self.PlaneWidgetZ == None:
            self.PlaneWidgetZ = vtkvmtk.vtkvmtkImagePlaneWidget()

        self.Picker.SetTolerance(0.005)

        self.PlaneWidgetX.SetResliceInterpolateToLinear()
        self.PlaneWidgetX.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetX.SetUseContinuousCursor(self.ContinuousCursor)
        self.PlaneWidgetX.SetInputData(self.Image)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetX.DisplayTextOn()
        else:
            self.PlaneWidgetX.DisplayTextOff()
        self.PlaneWidgetX.SetPicker(self.Picker)
        self.PlaneWidgetX.KeyPressActivationOff()
        self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        if self.Margins:
            self.PlaneWidgetX.SetMarginSizeX(0.05)
            self.PlaneWidgetX.SetMarginSizeY(0.05)
        else:
            self.PlaneWidgetX.SetMarginSizeX(0.0)
            self.PlaneWidgetX.SetMarginSizeY(0.0)
        if self.WindowLevel[0] != 0.0:
            self.PlaneWidgetX.SetWindowLevel(self.WindowLevel[0],self.WindowLevel[1])
        self.PlaneWidgetX.On()

        self.PlaneWidgetY.SetResliceInterpolateToLinear()
        self.PlaneWidgetY.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetY.SetUseContinuousCursor(self.ContinuousCursor)
        self.PlaneWidgetY.SetInputData(self.Image)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetY.DisplayTextOn()
        else:
            self.PlaneWidgetY.DisplayTextOff()
        self.PlaneWidgetY.SetPicker(self.Picker)
        self.PlaneWidgetY.KeyPressActivationOff()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        if self.Margins:
            self.PlaneWidgetY.SetMarginSizeX(0.05)
            self.PlaneWidgetY.SetMarginSizeY(0.05)
        else:
            self.PlaneWidgetY.SetMarginSizeX(0.0)
            self.PlaneWidgetY.SetMarginSizeY(0.0)
        if self.WindowLevel[0] != 0.0:
            self.PlaneWidgetY.SetWindowLevel(self.WindowLevel[0],self.WindowLevel[1])
        self.PlaneWidgetY.On()

        self.PlaneWidgetZ.SetResliceInterpolateToLinear()
        self.PlaneWidgetZ.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetZ.SetUseContinuousCursor(self.ContinuousCursor)
        self.PlaneWidgetZ.SetInputData(self.Image)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        if self.vmtkRenderer.Annotations:
            self.PlaneWidgetZ.DisplayTextOn()
        else:
            self.PlaneWidgetZ.DisplayTextOff()
        self.PlaneWidgetZ.SetPicker(self.Picker)
        self.PlaneWidgetZ.KeyPressActivationOff()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())
        self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        if self.Margins:
            self.PlaneWidgetZ.SetMarginSizeX(0.05)
            self.PlaneWidgetZ.SetMarginSizeY(0.05)
        else:
            self.PlaneWidgetZ.SetMarginSizeX(0.0)
            self.PlaneWidgetZ.SetMarginSizeY(0.0)
        if self.WindowLevel[0] != 0.0:
            self.PlaneWidgetZ.SetWindowLevel(self.WindowLevel[0],self.WindowLevel[1])
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
