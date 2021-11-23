#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagemipviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/06/20 12:12:05 $
## Version:   $Revision: 1.1 $

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


class vmtkImageMIPViewer(pypes.pypeScript):
    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.SampleDistance = 1.0
        self.AutoSampleDistance = 1

        self.WindowLevel = [0.0, 0.0]
        self.Opacity = None

        self.Volume = None

        self.SetScriptName('vmtkimagemipviewer')
        self.SetScriptDoc('display a 3D image')
        self.SetInputMembers([
            ['Image', 'i', 'vtkImageData', 1, '', 'the input image', 'vmtkimagereader'],
            ['ArrayName', 'array', 'str', 1, '', 'name of the array to display'],
            ['vmtkRenderer', 'renderer', 'vmtkRenderer', 1, '', 'external renderer'],
            ['SampleDistance', 'sampledistance', 'float', 1, '(0.0,)',
             'the distance at sample projections are generated'],
            ['AutoSampleDistance', 'autosampledistance', 'bool', 1, '', 'toggle automatic sample distance'],
            ['WindowLevel', 'windowlevel', 'float', 2, '', 'the window/level for generating the rendering'],
            ['Opacity', 'opacity', 'float', 2, '', 'the opacity range is defined from 0 (transparent) to 1 (opaque)'],
            ['Display', 'display', 'bool', 1, '', 'toggle rendering']
        ])
        self.SetOutputMembers([
            ['Image', 'o', 'vtkImageData', 1, '', 'the output image', 'vmtkimagewriter']
        ])

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if self.Volume:
            self.vmtkRenderer.Renderer.RemoveVolume(self.Volume)

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)

        scalarRange = [0.0, 0.0]

        if self.WindowLevel[0] > 0.0:
            scalarRange = [self.WindowLevel[1] - self.WindowLevel[0] / 2.0,
                           self.WindowLevel[1] + self.WindowLevel[0] / 2.0]
        else:
            scalarRange = self.Image.GetScalarRange()

        colorTransferFunction = vtk.vtkColorTransferFunction()
        colorTransferFunction.AddRGBPoint(scalarRange[0], 0.0, 0.0, 0.0)
        colorTransferFunction.AddRGBPoint(scalarRange[1], 1.0, 1.0, 1.0)

        volumeMapper = vtk.vtkFixedPointVolumeRayCastMapper()
        volumeMapper.SetInputData(self.Image)
        volumeMapper.SetBlendModeToMaximumIntensity()
        if self.AutoSampleDistance:
            volumeMapper.AutoAdjustSampleDistancesOn()
        else:
            volumeMapper.SetSampleDistance(self.SampleDistance)

        opacityFunction = vtk.vtkPiecewiseFunction()
        imageRange = self.Image.GetScalarRange()

        if self.Opacity is not None:
            opacityFunction.AddPoint(imageRange[0], 0)
            opacityFunction.AddPoint(scalarRange[0], self.Opacity[0])
            opacityFunction.AddPoint(scalarRange[0], self.Opacity[1])
            opacityFunction.AddPoint(imageRange[1], 1)

        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.ShadeOn()
        volumeProperty.SetInterpolationTypeToLinear()
        volumeProperty.SetColor(colorTransferFunction)
        if self.Opacity is not None:
            volumeProperty.SetScalarOpacity(opacityFunction)

        self.Volume = vtk.vtkVolume()
        self.Volume.SetMapper(volumeMapper)
        self.Volume.SetProperty(volumeProperty)

        self.vmtkRenderer.Renderer.AddVolume(self.Volume)

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')

        self.BuildView()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
