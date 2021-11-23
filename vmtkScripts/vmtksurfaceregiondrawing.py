#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceregiondrawing.py,v $
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


class vmtkSurfaceRegionDrawing(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Actor = None
        self.ContourWidget = None
        self.Interpolator = None
        self.Binary = 1
        self.OutsideValue = 1.0
        self.InsideValue = 0.0

        self.ContourScalarsArrayName = 'ContourScalars'

        self.SetScriptName('vmtksurfaceregiondrawing')
        self.SetScriptDoc('draw a closed contour on a surface and generate a distance field on the surface')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['Binary','binary','bool',1,'','fill contour with inside value instead of distance to contour'],
            ['InsideValue','inside','float',1,'','value with which the surface within the contour is filled'],
            ['OutsideValue','outside','float',1,'','value with which the surface outside the contour is filled'],
            ['ContourScalarsArrayName','array','str',1,'','the name of the array where the generated scalars are stored'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def ScalarsCallback(self, obj):

        rep = vtk.vtkOrientedGlyphContourRepresentation.SafeDownCast(self.ContourWidget.GetRepresentation())

        pointIds = vtk.vtkIdList()
        self.Interpolator.GetContourPointIds(rep,pointIds)

        points = vtk.vtkPoints()
        points.SetNumberOfPoints(pointIds.GetNumberOfIds())

        for i in range(pointIds.GetNumberOfIds()):
            pointId = pointIds.GetId(i)
            point = self.Surface.GetPoint(pointId)
            points.SetPoint(i,point)

        selectionFilter = vtk.vtkSelectPolyData()
        selectionFilter.SetInputData(self.Surface)
        selectionFilter.SetLoop(points)
        selectionFilter.GenerateSelectionScalarsOn()
        selectionFilter.SetSelectionModeToSmallestRegion()
        selectionFilter.Update()

        selectionScalars = selectionFilter.GetOutput().GetPointData().GetScalars()

        contourScalars = self.Surface.GetPointData().GetArray(self.ContourScalarsArrayName)

        for i in range(contourScalars.GetNumberOfTuples()):
            selectionValue = selectionScalars.GetTuple1(i)
            if self.Binary:
                if selectionValue < 0.0:
                    contourScalars.SetTuple1(i,self.InsideValue)
            else:
                contourValue = contourScalars.GetTuple1(i)
                if (not contourValue < 0.0 and selectionValue < 0.0) or (contourValue < 0.0 and selectionValue < contourValue):
                    contourScalars.SetTuple1(i,selectionValue)

        self.Actor.GetMapper().SetScalarRange(contourScalars.GetRange(0))

        self.Surface.Modified()

        self.ContourWidget.Initialize()

    def DeleteContourCallback(self, obj):
        self.ContourWidget.Initialize()

    def InteractCallback(self, obj):
        if self.ContourWidget.GetEnabled() == 1:
            self.ContourWidget.SetEnabled(0)
        else:
            self.ContourWidget.SetEnabled(1)

    def Display(self):

        self.vmtkRenderer.Render()

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        triangleFilter = vtk.vtkTriangleFilter()
        triangleFilter.SetInputData(self.Surface)
        triangleFilter.Update()

        self.Surface = triangleFilter.GetOutput()

        contourScalars = vtk.vtkDoubleArray()
        contourScalars.SetNumberOfComponents(1)
        contourScalars.SetNumberOfTuples(self.Surface.GetNumberOfPoints())
        contourScalars.SetName(self.ContourScalarsArrayName)
        contourScalars.FillComponent(0,self.OutsideValue)

        self.Surface.GetPointData().AddArray(contourScalars)
        self.Surface.GetPointData().SetActiveScalars(self.ContourScalarsArrayName)

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(self.Surface)
        mapper.ScalarVisibilityOn()
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.Actor.GetMapper().SetScalarRange(-1.0,0.0)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)

        self.ContourWidget = vtk.vtkContourWidget()
        self.ContourWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        rep = vtk.vtkOrientedGlyphContourRepresentation.SafeDownCast(self.ContourWidget.GetRepresentation())
        rep.GetLinesProperty().SetColor(1, 0.2, 0)
        rep.GetLinesProperty().SetLineWidth(3.0)

        pointPlacer = vtk.vtkPolygonalSurfacePointPlacer()
        pointPlacer.AddProp(self.Actor)
        pointPlacer.GetPolys().AddItem(self.Surface)
        rep.SetPointPlacer(pointPlacer)

        self.Interpolator = vtk.vtkPolygonalSurfaceContourLineInterpolator()
        self.Interpolator.GetPolys().AddItem(self.Surface)
        rep.SetLineInterpolator(self.Interpolator)

        self.vmtkRenderer.AddKeyBinding('space','Generate scalars',self.ScalarsCallback)
        self.vmtkRenderer.AddKeyBinding('d','Delete contour',self.DeleteContourCallback)
        self.vmtkRenderer.AddKeyBinding('i','Start interaction',self.InteractCallback)
        self.Display()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
