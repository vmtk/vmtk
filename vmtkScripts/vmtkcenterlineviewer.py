#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlineviewer.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.3 $

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

vmtkcenterlineviewer = 'vmtkCenterlineViewer'

class vmtkCenterlineViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Centerlines = None
        self.PointDataArrayName = ''
        self.CellDataArrayName = ''

        self.Legend = 1
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtkcenterlineviewer')
        self.SetScriptDoc('')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['PointDataArrayName','pointarray','str',1,''],
            ['CellDataArrayName','cellarray','str',1,''],
            ['Legend','legend','bool',1,''],
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

        if self.CellDataArrayName:
            cellCenters = vtk.vtkCellCenters()
            cellCenters.SetInput(self.Centerlines)
            cellCenters.Update()
            cellCenters.GetOutput().GetPointData().SetActiveScalars(self.CellDataArrayName)
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInput(cellCenters.GetOutput())
            labelsMapper.SetLabelModeToLabelScalars()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)
            self.vmtkRenderer.Renderer.AddActor(labelsActor)

        centerlineMapper = vtk.vtkPolyDataMapper()
        centerlineMapper.SetInput(self.Centerlines)
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

