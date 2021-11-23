#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshviewer.py,v $
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


class vmtkMeshViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.Opacity = 1.0
        self.ArrayName = ''
        self.ScalarRange = [0.0, 0.0]
        self.ColorMap = 'cooltowarm'
        self.NumberOfColors = 256
        self.Color = [-1.0, -1.0, -1.0]
        self.Legend = 0
        self.FlatInterpolation = 0

        self.Representation = 'surface'

        self.Actor = None
        self.ScalarBarActor = None

        self.SetScriptName('vmtkmeshviewer')
        self.SetScriptDoc('display a mesh')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacity in the scene'],
            ['ArrayName','array','str',1,'','name of the array where the scalars to be displayed are stored'],
            ['ScalarRange','scalarrange','float',2,'','range of the scalar map'],
            ['ColorMap','colormap','str',1,'["rainbow","blackbody","cooltowarm","grayscale"]','choose the color map'],
            ['NumberOfColors','numberofcolors','int',1,'','number of colors in the color map'],
            ['Legend','legend','bool',1,'','toggle scalar bar'],
            ['Color','color','float',3,'','RGB color of the object in the scene'],
            ['FlatInterpolation','flat','bool',1,'','toggle flat or shaded surface display']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def RepresentationCallback(self, obj):
        if not self.Actor:
            return

        if self.Representation == 'surface':
            self.Representation = 'edges'
        elif self.Representation == 'edges':
            self.Representation = 'wireframe'
        elif self.Representation == 'wireframe':
            self.Representation = 'surface'

        if self.Representation == 'surface':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOff()
        elif self.Representation == 'edges':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOn()
        elif self.Representation == 'wireframe':
            self.Actor.GetProperty().SetRepresentationToWireframe()
            self.Actor.GetProperty().EdgeVisibilityOff()

        self.vmtkRenderer.RenderWindow.Render()

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if self.Actor:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.ScalarBarActor:
            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.Mesh:
            mapper = vtk.vtkDataSetMapper()
            mapper.SetInputData(self.Mesh)
            if self.ArrayName:
                self.Mesh.GetPointData().SetActiveScalars(self.ArrayName)
                array = self.Mesh.GetPointData().GetScalars()

                if self.ScalarRange[1] > self.ScalarRange[0]:
                    mapper.SetScalarRange(self.ScalarRange)
                else:
                    mapper.SetScalarRange(array.GetRange(0))

                if self.ColorMap == 'grayscale':
                    lut = mapper.GetLookupTable()
                    lut.SetNumberOfTableValues(self.NumberOfColors)
                    lut.SetValueRange(0.0,1.0)
                    lut.SetSaturationRange(0.0,0.0)
                    lut.Build()
                    mapper.SetLookupTable(lut)

                if self.ColorMap == 'rainbow':
                    lut = mapper.GetLookupTable()
                    lut.SetHueRange(0.666667,0.0)
                    lut.SetSaturationRange(0.75,0.75)
                    lut.SetValueRange(1.0,1.0)
                    lut.SetAlphaRange(1.0,1.0)
                    lut.SetNumberOfColors(self.NumberOfColors)
                    lut.Build()
                    mapper.SetLookupTable(lut)

                if self.ColorMap == 'blackbody':
                   lut = mapper.GetLookupTable()
                   lut.SetNumberOfTableValues(self.NumberOfColors)
                   colorTransferFunction = vtk.vtkColorTransferFunction()
                   colorTransferFunction.SetColorSpaceToRGB()
                   colorTransferFunction.AddRGBPoint(0,0.0,0.0,0.0)
                   colorTransferFunction.AddRGBPoint(0.4,0.901961,0.0,0.0)
                   colorTransferFunction.AddRGBPoint(0.8,0.901961,0.901961,0.0)
                   colorTransferFunction.AddRGBPoint(1.0,1.0,1.0,1.0)
                   for ii,ss in enumerate([float(xx)/float(self.NumberOfColors) for xx in range(self.NumberOfColors)]):
                       cc = colorTransferFunction.GetColor(ss)
                       lut.SetTableValue(ii,cc[0],cc[1],cc[2],1.0)
                   lut.Build()
                   mapper.SetLookupTable(lut)

                if self.ColorMap == 'cooltowarm':
                   lut = mapper.GetLookupTable()
                   lut.SetNumberOfTableValues(self.NumberOfColors)
                   colorTransferFunction = vtk.vtkColorTransferFunction()
                   colorTransferFunction.SetColorSpaceToDiverging()
                   colorTransferFunction.AddRGBPoint(0,0.231373,0.298039,0.752941)
                   colorTransferFunction.AddRGBPoint(0.5,0.865003,0.865003,0.865003)
                   colorTransferFunction.AddRGBPoint(1.0,0.705882,0.0156863,0.14902)
                   for ii,ss in enumerate([float(xx)/float(self.NumberOfColors) for xx in range(self.NumberOfColors)]):
                       cc = colorTransferFunction.GetColor(ss)
                       lut.SetTableValue(ii,cc[0],cc[1],cc[2],1.0)
                   lut.Build()
                   mapper.SetLookupTable(lut)
            else:
                mapper.ScalarVisibilityOff()
            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            if (self.Color[0] >= 0.0):
                self.Actor.GetProperty().SetColor(self.Color)
            if self.FlatInterpolation:
                self.Actor.GetProperty().SetInterpolationToFlat()
            self.Actor.GetProperty().SetOpacity(self.Opacity)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)
            self.vmtkRenderer.AddKeyBinding('w','Change surface representation.',self.RepresentationCallback)

        if self.Legend and self.Actor:
            self.ScalarBarActor = vtk.vtkScalarBarActor()
            self.ScalarBarActor.SetLookupTable(self.Actor.GetMapper().GetLookupTable())
            self.ScalarBarActor.GetLabelTextProperty().ItalicOff()
            self.ScalarBarActor.GetLabelTextProperty().BoldOff()
            self.ScalarBarActor.GetLabelTextProperty().ShadowOff()
##             self.ScalarBarActor.GetLabelTextProperty().SetColor(0.0,0.0,0.0)
            self.ScalarBarActor.SetLabelFormat('%.2f')
            self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        if self.Display:
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Mesh == None) & (self.Display == 1):
            self.PrintError('Error: no Mesh.')

        self.BuildView()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
