#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.10 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import sys

import vmtkrenderer
import pypes

vmtksurfaceviewer = 'vmtkSurfaceViewer'

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
        self.Legend = 0
        self.LegendTitle = ''
        self.Grayscale = 0
        self.FlatInterpolation = 0
        self.DisplayCellData = 0
        self.Color = [-1.0, -1.0, -1.0]
        self.LineWidth = 1

        self.Actor = None
        self.ScalarBarActor = None

        self.SetScriptName('vmtksurfaceviewer')
        self.SetScriptDoc('display a surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Opacity','opacity','float',1,'(0.0,1.0)','obejct opacity in the scene'],
            ['ArrayName','array','str',1,'','name of the array where the scalars to be displayed are stored'],
            ['ScalarRange','scalarrange','float',2,'','range of the scalar map'],
            ['Legend','legend','bool',1,'','toggle scalar bar'],
            ['Grayscale','grayscale','bool',1,'','toggle color or grayscale'],
            ['FlatInterpolation','flat','bool',1,'','toggle flat or shaded surface display'],
            ['DisplayCellData','celldata','bool',1,'','toggle display of point or cell data'],
            ['Color','color','float',3,'','RGB color of the object in the scene'],
            ['LineWidth','linewidth','int',1,'(0.0,)','width of line objects in the scene'],
            ['LegendTitle','legendtitle','str',1,'','title of the scalar bar']])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        if self.Actor:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.ScalarBarActor:
            self.vmtkRenderer.Renderer.RemoveActor(self.ScalarBarActor)

        if self.Surface:
            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInput(self.Surface)
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
                    array.ComputeRange(0)
                    mapper.SetScalarRange(array.GetRange(0))
                if self.Grayscale:
                    lut = vtk.vtkLookupTable()
                    lut.SetValueRange(0.0,1.0)
                    lut.SetSaturationRange(0.0,0.0)
                    mapper.SetLookupTable(lut)
            else:
                mapper.ScalarVisibilityOff()
            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            if (self.Color[0] >= 0.0):
                self.Actor.GetProperty().SetColor(self.Color)
            self.Actor.GetProperty().SetOpacity(self.Opacity)
            self.Actor.GetProperty().SetLineWidth(self.LineWidth)
            if self.FlatInterpolation:
                self.Actor.GetProperty().SetInterpolationToFlat()
            self.vmtkRenderer.Renderer.AddActor(self.Actor)

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

        self.BuildView()
        
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
