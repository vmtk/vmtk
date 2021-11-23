#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlineviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.3 $

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


class vmtkCenterlineViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.PointDataArrayName = ''
        self.CellDataArrayName = ''
        self.Display = 1
        self.Legend = 1
        self.ColorMap = 'cooltowarm'
        self.NumberOfColors = 256
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtkcenterlineviewer')
        self.SetScriptDoc('display a 3D centerline')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['PointDataArrayName','pointarray','str',1,''],
            ['CellDataArrayName','cellarray','str',1,''],
            ['Legend','legend','bool',1,''],
            ['ColorMap','colormap','str',1,'["rainbow","blackbody","cooltowarm","grayscale"]','choose the color map'],
            ['NumberOfColors','numberofcolors','int',1,'','number of colors in the color map'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']])

    def Execute(self):

        if not self.Centerlines:
            self.PrintError('Error: No input centerlines.')
            return

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if self.CellDataArrayName:
            cellCenters = vtk.vtkCellCenters()
            cellCenters.SetInputData(self.Centerlines)
            cellCenters.Update()
            cellCenters.GetOutput().GetPointData().SetActiveScalars(self.CellDataArrayName)
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInputConnection(cellCenters.GetOutputPort())
            labelsMapper.SetLabelModeToLabelScalars()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)
            self.vmtkRenderer.Renderer.AddActor(labelsActor)

        centerlineMapper = vtk.vtkPolyDataMapper()
        centerlineMapper.SetInputData(self.Centerlines)
        if self.CellDataArrayName and not self.PointDataArrayName:
            centerlineMapper.ScalarVisibilityOn()
            centerlineMapper.SetScalarModeToUseCellData()
            self.Centerlines.GetCellData().SetActiveScalars(self.CellDataArrayName)
            centerlineMapper.SetScalarRange(self.Centerlines.GetCellData().GetScalars().GetRange(0))
        elif self.PointDataArrayName:
            centerlineMapper.ScalarVisibilityOn()
            centerlineMapper.SetScalarModeToUsePointData()
            self.Centerlines.GetPointData().SetActiveScalars(self.PointDataArrayName)
            centerlineMapper.SetScalarRange(self.Centerlines.GetPointData().GetScalars().GetRange(0))
        else:
            centerlineMapper.ScalarVisibilityOff()

        if self.ColorMap == 'grayscale':
            lut = centerlineMapper.GetLookupTable()
            lut.SetNumberOfTableValues(self.NumberOfColors)
            lut.SetValueRange(0.0,1.0)
            lut.SetSaturationRange(0.0,0.0)
            lut.Build()
            centerlineMapper.SetLookupTable(lut)

        if self.ColorMap == 'rainbow':
            lut = centerlineMapper.GetLookupTable()
            lut.SetHueRange(0.666667,0.0)
            lut.SetSaturationRange(0.75,0.75)
            lut.SetValueRange(1.0,1.0)
            lut.SetAlphaRange(1.0,1.0)
            lut.SetNumberOfColors(self.NumberOfColors)
            lut.Build()
            centerlineMapper.SetLookupTable(lut)

        if self.ColorMap == 'blackbody':
            lut = centerlineMapper.GetLookupTable()
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
            centerlineMapper.SetLookupTable(lut)

        if self.ColorMap == 'cooltowarm':
            lut = centerlineMapper.GetLookupTable()
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
            centerlineMapper.SetLookupTable(lut)

        centerlineActor = vtk.vtkActor()
        centerlineActor.SetMapper(centerlineMapper)
        self.vmtkRenderer.Renderer.AddActor(centerlineActor)

        scalarBarActor = None
        if self.Legend and centerlineActor and self.PointDataArrayName:
            scalarBarActor = vtk.vtkScalarBarActor()
            scalarBarActor.SetLookupTable(centerlineActor.GetMapper().GetLookupTable())
            scalarBarActor.GetLabelTextProperty().ItalicOff()
            scalarBarActor.GetLabelTextProperty().BoldOff()
            scalarBarActor.GetLabelTextProperty().ShadowOff()
            scalarBarActor.SetLabelFormat('%.2f')
            scalarBarActor.SetTitle(self.PointDataArrayName)
            self.vmtkRenderer.Renderer.AddActor(scalarBarActor)

        if self.Display:
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

#        if self.CellDataArrayName:
#            self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
#
#        if self.Legend and centerlineActor:
#            self.vmtkRenderer.Renderer.RemoveActor(scalarBarActor)
#
#        self.vmtkRenderer.Renderer.RemoveActor(centerlineActor)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
