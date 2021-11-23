#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

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


class vmtkSurfaceViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.Opacity = 1.0
        self.ArrayName = ''
        self.ScalarRange = [0.0, 0.0]
        self.ColorMap = 'cooltowarm'
        self.NumberOfColors = 256
        self.Legend = 0
        self.LegendTitle = ''
        self.Grayscale = 0
        self.FlatInterpolation = 0
        self.DisplayCellData = 0
        self.Color = [-1.0, -1.0, -1.0]
        self.LineWidth = 1
        self.Representation = 'surface'
        self.DisplayTag = False
        self.RegionTagArrayName = 'RegionTagArray'
        self.NumberOfRegions = 0
        self.TagSet = []

        self.Actor = None
        self.ScalarBarActor = None

        self.SetScriptName('vmtksurfaceviewer')
        self.SetScriptDoc('display a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Representation','representation','str',1,'["surface","wireframe","edges"]','change surface representation'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','object opacity in the scene'],
            ['ArrayName','array','str',1,'','name of the array where the scalars to be displayed are stored'],
            ['ScalarRange','scalarrange','float',2,'','range of the scalar map'],
            ['ColorMap','colormap','str',1,'["rainbow","blackbody","cooltowarm","grayscale"]','choose the color map'],
            ['NumberOfColors','numberofcolors','int',1,'','number of colors in the color map'],
            ['Legend','legend','bool',1,'','toggle scalar bar'],
            ['FlatInterpolation','flat','bool',1,'','toggle flat or shaded surface display'],
            ['DisplayCellData','celldata','bool',1,'','toggle display of point or cell data'],
            ['DisplayTag','displaytag','bool',1,'','toggle rendering of tag'],
            ['RegionTagArrayName','regiontagarray','str',1,'','name of the array where the tags to be displayed are stored'],
            ['Color','color','float',3,'','RGB color of the object in the scene'],
            ['LineWidth','linewidth','int',1,'(0.0,)','width of line objects in the scene'],
            ['LegendTitle','legendtitle','str',1,'','title of the scalar bar']])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter'],
            ['Actor','oactor','vtkActor',1,'','the output actor']
            ])

    def SetSurfaceRepresentation(self, representation):
        if representation == 'surface':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOff()
        elif representation == 'edges':
            self.Actor.GetProperty().SetRepresentationToSurface()
            self.Actor.GetProperty().EdgeVisibilityOn()
        elif representation == 'wireframe':
            self.Actor.GetProperty().SetRepresentationToWireframe()
            self.Actor.GetProperty().EdgeVisibilityOff()
        self.Representation = representation

    def RepresentationCallback(self, obj):
        if not self.Actor:
            return

        if self.Representation == 'surface':
            representation = 'edges'
        elif self.Representation == 'edges':
            representation = 'wireframe'
        elif self.Representation == 'wireframe':
            representation = 'surface'

        self.SetSurfaceRepresentation(representation)

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

        if self.Surface:
            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInputData(self.Surface)
            if self.ArrayName:
                if self.DisplayCellData == 0:
                    self.Surface.GetPointData().SetActiveScalars(self.ArrayName)
                    array = self.Surface.GetPointData().GetScalars()
                else:
                    self.Surface.GetCellData().SetActiveScalars(self.ArrayName)
                    array = self.Surface.GetCellData().GetScalars()
                    mapper.SetScalarModeToUseCellData()
                if (self.ScalarRange[1] > self.ScalarRange[0]):
                    mapper.SetScalarRange(self.ScalarRange)
                elif array:
                    mapper.SetScalarRange(array.GetRange(0))
                if self.Grayscale:
                    lut = vtk.vtkLookupTable()
                    lut.SetValueRange(0.0,1.0)
                    lut.SetSaturationRange(0.0,0.0)
                    mapper.SetLookupTable(lut)
            else:
                mapper.ScalarVisibilityOff()

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

            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            if (self.Color[0] >= 0.0):
                self.Actor.GetProperty().SetColor(self.Color)
            self.Actor.GetProperty().SetOpacity(self.Opacity)
            self.Actor.GetProperty().SetLineWidth(self.LineWidth)
            if self.FlatInterpolation:
                self.Actor.GetProperty().SetInterpolationToFlat()
            self.SetSurfaceRepresentation(self.Representation)
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
            self.ScalarBarActor.SetTitle(self.LegendTitle)
            self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        if self.Display:
            self.vmtkRenderer.Render()
##            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)
##            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def BuildViewWithTag(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if self.Actor:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.ScalarBarActor:
            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.Surface.GetPointData().GetArray(self.RegionTagArrayName) == None and self.Surface.GetCellData().GetArray(self.RegionTagArrayName) == None:
            self.PrintError('Error: no regiontagarray with name specified')

        elif self.Surface.GetPointData().GetArray(self.RegionTagArrayName) != None:

            regionTagArray = self.Surface.GetPointData().GetArray(self.RegionTagArrayName)
            for j in range (self.Surface.GetNumberOfPoints()):
                if regionTagArray.GetTuple1(j) not in self.TagSet:
                    self.TagSet.append(regionTagArray.GetTuple1(j))
            self.TagSet.sort()
            self.NumberOfRegions = len(self.TagSet)

            tagSetCopy = list(self.TagSet)
            labelPoints = vtk.vtkPoints()
            labelPoints.SetNumberOfPoints(len(self.TagSet))
            point = [0.0,0.0,0.0]
            for j in range (self.Surface.GetNumberOfPoints()):
                item = regionTagArray.GetTuple1(j)
                if item in tagSetCopy:
                    self.Surface.GetPoint(j, point)
                    labelPoints.SetPoint(self.TagSet.index(item), point)
                    tagSetCopy.remove(item)

            self.Surface.GetPointData().SetActiveScalars(self.RegionTagArrayName)

        elif self.Surface.GetCellData().GetArray(self.RegionTagArrayName) != None:

            regionTagArray = self.Surface.GetCellData().GetArray(self.RegionTagArrayName)
            for j in range (self.Surface.GetNumberOfCells()):
                if regionTagArray.GetTuple1(j) not in self.TagSet:
                    self.TagSet.append(regionTagArray.GetTuple1(j))
            self.TagSet.sort()
            self.NumberOfRegions = len(self.TagSet)

            tagSetCopy = list(self.TagSet)
            labelPoints = vtk.vtkPoints()
            labelPoints.SetNumberOfPoints(len(self.TagSet))
            point = [0.0,0.0,0.0]

            cellCenters = vtk.vtkCellCenters()
            cellCenters.SetInputData(self.Surface)
            cellCenters.Update()

            regionTagArrayCenters = cellCenters.GetOutput().GetPointData().GetArray(self.RegionTagArrayName)

            for j in range (cellCenters.GetOutput().GetNumberOfPoints()):
                item = regionTagArrayCenters.GetTuple1(j)
                if item in tagSetCopy:
                    cellCenters.GetOutput().GetPoint(j, point)
                    labelPoints.SetPoint(self.TagSet.index(item), point)
                    tagSetCopy.remove(item)

            self.Surface.GetCellData().SetActiveScalars(self.RegionTagArrayName)

        labelPolyData = vtk.vtkPolyData()
        labelPolyData.SetPoints(labelPoints)

        labelArray = vtk.vtkIntArray()
        labelArray.SetNumberOfComponents(1)
        labelArray.SetNumberOfTuples(self.NumberOfRegions)
        labelArray.SetName('label')
        labelArray.FillComponent(0,0)

        labelPolyData.GetPointData().AddArray(labelArray)

        for item in self.TagSet:
            labelArray.SetTuple1(self.TagSet.index(item), item)

        labelPolyData.GetPointData().SetActiveScalars('label')
        labelsMapper = vtk.vtkLabeledDataMapper()
        labelsMapper.SetInputData(labelPolyData)
        labelsMapper.SetLabelModeToLabelScalars()
        labelsMapper.GetLabelTextProperty().SetColor(1, 1, 1)
        labelsMapper.GetLabelTextProperty().SetFontSize(14)
        self.labelsActor = vtk.vtkActor2D()
        self.labelsActor.SetMapper(labelsMapper)
        self.vmtkRenderer.Renderer.AddActor(self.labelsActor)

        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputData(self.Surface)
        surfaceMapper.ScalarVisibilityOn()
        surfaceMapper.SetScalarRange(self.TagSet[0], self.TagSet[self.NumberOfRegions-1])
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(surfaceMapper)
        self.Actor.GetProperty().SetOpacity(self.Opacity)
        self.Actor.GetProperty().SetLineWidth(self.LineWidth)
        if self.FlatInterpolation:
            self.Actor.GetProperty().SetInterpolationToFlat()
        self.SetSurfaceRepresentation(self.Representation)
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
            self.ScalarBarActor.SetTitle(self.LegendTitle)
            self.vmtkRenderer.Renderer.AddActor(self.ScalarBarActor)

        if self.Display:
            self.vmtkRenderer.Render()
##            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)
##            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):
        if (not self.Surface) and self.Display:
            self.PrintError('Error: no Surface.')

        if not self.DisplayTag:
            self.BuildView()
        else:
            self.BuildViewWithTag()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
