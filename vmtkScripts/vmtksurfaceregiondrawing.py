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

        self.Representation = 'edges'
        self.Actor = None
        self.ContourWidget = None
        self.Interpolator = None
        self.OutsideValue = 0.0
        self.InsideValue = 1.0
        self.OverwriteOutsideValue = 0
        self.ComputeDistance = 0
        self.TagSmallestRegion = 1

        self.CellData = 1

        self.ArrayName = 'CellEntityIds'
        self.Array = None

        self.SetScriptName('vmtksurfaceregiondrawing')
        self.SetScriptDoc('draw a closed contour on a surface and generate a new tag inside it')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['CellData','celldata','bool',1,'','toggle writing point or cell data array'],
            ['InsideValue','inside','float',1,'','value with which the surface within the contour is filled'],
            ['OutsideValue','outside','float',1,'','value with which the surface outside the contour is filled'],
            ['OverwriteOutsideValue','overwriteoutside','bool',1,'','overwrite outside value also when a tag array already exists in the input surface'],
            ['ArrayName','array','str',1,'','the name of the self.Array where the generated scalars are stored'],
            ['TagSmallestRegion','tagsmallestregion','bool',1,'','toggle tagging the smallest or largest region'],
            ['ComputeDistance','computedistance','bool',1,'','fill the array with the distance to the contour'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
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
        if self.TagSmallestRegion:
            selectionFilter.SetSelectionModeToSmallestRegion()
        else:
            selectionFilter.SetSelectionModeToLargestRegion()
        selectionFilter.Update()

        selectionScalars = selectionFilter.GetOutput().GetPointData().GetScalars()
        selectionScalars.SetName('SelectionFilter')

        if self.CellData:
            self.Surface.GetPointData().AddArray(selectionScalars)
            pointDataToCellDataFilter = vtk.vtkPointDataToCellData()
            pointDataToCellDataFilter.SetInputData(self.Surface)
            pointDataToCellDataFilter.PassPointDataOn()
            pointDataToCellDataFilter.Update()
            self.Surface = pointDataToCellDataFilter.GetPolyDataOutput()
            selectionScalars = self.Surface.GetCellData().GetArray('SelectionFilter')

        for i in range(self.Array.GetNumberOfTuples()):
            selectionValue = selectionScalars.GetTuple1(i)
            if self.ComputeDistance:
                contourValue = self.Array.GetTuple1(i)
                if (not contourValue < 0.0 and selectionValue < 0.0) or (contourValue < 0.0 and selectionValue < contourValue):
                    self.Array.SetTuple1(i,selectionValue)
            else:
                if selectionValue <= 0.0:
                    self.Array.SetTuple1(i,self.InsideValue)

        if self.CellData:
            self.Surface.GetPointData().RemoveArray('SelectionFilter')
            self.Surface.GetCellData().RemoveArray('SelectionFilter')

        self.Surface.Modified()
        self.ContourWidget.Initialize()

    def DeleteContourCallback(self, obj):
        self.ContourWidget.Initialize()

    def InteractCallback(self, obj):
        # BUG: enable the widget, but immediately after it is disabled again
        if self.ContourWidget.GetEnabled() == 1:
            self.ContourWidget.SetEnabled(0)
        else:
            self.ContourWidget.SetEnabled(1)

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

        if self.CellData:
            self.Array = self.Surface.GetCellData().GetArray(self.ArrayName)
        else:
            self.Array = self.Surface.GetPointData().GetArray(self.ArrayName)

        if self.Array == None or self.OverwriteOutsideValue:
            self.Array = vtk.vtkDoubleArray()
            self.Array.SetNumberOfComponents(1)
            if self.CellData:
                self.Array.SetNumberOfTuples(self.Surface.GetNumberOfCells())
            else:
                self.Array.SetNumberOfTuples(self.Surface.GetNumberOfPoints())
            self.Array.SetName(self.ArrayName)
            self.Array.FillComponent(0,self.OutsideValue)
            if self.CellData:
                self.Surface.GetCellData().AddArray(self.Array)
            else:
                self.Surface.GetPointData().AddArray(self.Array)

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(self.Surface)
        mapper.ScalarVisibilityOn()

        if self.CellData:
            self.Surface.GetCellData().SetActiveScalars(self.ArrayName)
            mapper.SetScalarModeToUseCellData()
        else:
            self.Surface.GetPointData().SetActiveScalars(self.ArrayName)
            mapper.SetScalarModeToUsePointData()

        arrayRange = [e for e in self.Array.GetValueRange(0)]
        if self.InsideValue > arrayRange[1]:
            arrayRange[1] = self.InsideValue
        elif self.InsideValue < arrayRange[0]:
            arrayRange[0] = self.InsideValue
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.Actor.GetMapper().SetScalarRange(arrayRange[0],arrayRange[1])
        self.vmtkRenderer.Renderer.AddActor(self.Actor)
        self.vmtkRenderer.Render()

        self.ContourWidget = vtk.vtkContourWidget()
        self.ContourWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)

        rep = vtk.vtkOrientedGlyphContourRepresentation.SafeDownCast(self.ContourWidget.GetRepresentation())
        rep.GetLinesProperty().SetColor(1, 0.2, 0)
        rep.GetLinesProperty().SetLineWidth(3.0)

        pointPlacer = vtk.vtkPolygonalSurfacePointPlacer()
        pointPlacer.AddProp(self.Actor)
        pointPlacer.GetPolys().AddItem(self.Surface)
        pointPlacer.SnapToClosestPointOn()
        rep.SetPointPlacer(pointPlacer)

        self.Interpolator = vtk.vtkPolygonalSurfaceContourLineInterpolator()
        self.Interpolator.GetPolys().AddItem(self.Surface)
        rep.SetLineInterpolator(self.Interpolator)

        self.ContourWidget.EnabledOn()
        self.InputInfo("Drawing contour ...\n")

        self.vmtkRenderer.AddKeyBinding('w','Change surface representation.',self.RepresentationCallback)
        self.vmtkRenderer.AddKeyBinding('space','Generate scalars',self.ScalarsCallback)
        self.vmtkRenderer.AddKeyBinding('d','Delete contour',self.DeleteContourCallback)
        #self.vmtkRenderer.AddKeyBinding('i','Start interaction',self.InteractCallback)
        self.vmtkRenderer.Render()



        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
