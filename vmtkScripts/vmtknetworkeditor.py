#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtknetworkeditor.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.3 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

##   Developed with support from the EC FP7/2007-2013: ARCH, Project n. 224390

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import vmtkimageviewer
from vmtk import vmtkimagefeatures
from vmtk import pypes

from vmtk import vmtkactivetubes


class vmtkNetworkEditor(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Network = None
        self.RadiusArrayName = 'Radius'
        self.LabelsArrayName = 'Labels'

        self.SplineInterpolation = 1

        self.UseActiveTubes = 0
        self.NumberOfIterations = 100
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 0.0

        self.NetworkTube = None

        self.Selection = None
        self.SelectionPoints = None
        self.SelectionRadiusArray = None
        self.SelectionActor = None
        self.SelectedCellId = -1
        self.SelectedSubId = -1
        self.SelectedPCoords = 0.0

        self.ActiveSegment = None
        self.ActiveSegmentPoints = None
        self.ActiveSegmentCellArray = None
        self.ActiveSegmentRadiusArray = None
        self.ActiveSegmentActor = None

        self.ActiveSegmentSeeds = None
        self.ActiveSegmentSeedsPoints = None
        self.ActiveSegmentSeedsRadiusArray = None
        self.ActiveSegmentSeedsActor = None

        self.AttachedCellIds = [-1,-1]
        self.AttachedSubIds = [-1,-1]
        self.AttachedPCoords = [0.0,0.0]

        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None

        self.Image = None
        self.FeatureImage = None

        self.CurrentRadius = 0.0

        self.NetworkRadiusArray = None
        self.NetworkLabelsArray = None

        self.CellPicker = None

        self.CellIdsToMerge = []

        self.OperationMode = None
        self.PickMode = 'image'

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtknetworkeditor')
        self.SetScriptDoc('extract an approximated centerline from a surface containing at least one hole')
        self.SetInputMembers([
            ['Network','i','vtkPolyData',1,'','the input network','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,''],
            ['LabelsArrayName','labelsarray','str',1,''],
            ['SplineInterpolation','spline','bool',1,''],
            ['UseActiveTubes','activetubes','bool',1,''],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['PotentialWeight','potentialweight','float',1,'(0.0,)'],
            ['StiffnessWeight','stiffnessweight','float',1,'(0.0,)'],
            ['PlaneWidgetX','xplane','vtkImagePlaneWidget',1,'','the X image plane widget'],
            ['PlaneWidgetY','yplane','vtkImagePlaneWidget',1,'','the Y image plane widget'],
            ['PlaneWidgetZ','zplane','vtkImagePlaneWidget',1,'','the Z image plane widget'],
            ['Image','image','vtkImageData',1,'','','vmtkimagereader'],
            ['FeatureImage','featureimage','vtkImageData',1,'','','vmtkimagereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['Network','o','vtkPolyData',1,'','the output network','vmtknetworkwriter'],
            ['RadiusArrayName','oradiusarray','str',1,''],
            ['Surface','osurface','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def TogglePickMode(self):
        if self.PickMode == 'image':
            self.PickMode = 'network'
        else:
            self.PickMode = 'image'

    def TogglePlaneWidget(self,planeWidget):
        if not planeWidget:
            return
        interaction = planeWidget.GetInteraction()
        if interaction:
            planeWidget.InteractionOff()
        else:
            planeWidget.InteractionOn()

    def UpdateLabels(self):
        self.CellCenters.Modified()
        self.CellCenters.Update()
        self.Render()

    def ToggleLabels(self):
        if self.LabelsActor.GetVisibility() == 1:
            self.LabelsActor.VisibilityOff()
        else:
            self.LabelsActor.VisibilityOn()
        self.Render()

    def SetPickMode(self,pickMode):
        if pickMode == 'image':
            self.PickMode = 'image'
            if self.PlaneWidgetX:
                self.PlaneWidgetX.InteractionOn()
            if self.PlaneWidgetY:
                self.PlaneWidgetY.InteractionOn()
            if self.PlaneWidgetZ:
                self.PlaneWidgetZ.InteractionOn()
        elif pickMode == 'network':
            self.PickMode = 'network'
            if self.PlaneWidgetX:
                self.PlaneWidgetX.InteractionOff()
            if self.PlaneWidgetY:
                self.PlaneWidgetY.InteractionOff()
            if self.PlaneWidgetZ:
                self.PlaneWidgetZ.InteractionOff()

    def KeyReleaseCallback(self,obj,event):
        pass
        #key = object.GetKeySym()
        #if self.OperationMode == 'add':
        #    if key == 'Shift_L' or key == 'Shift_R':
        #        if self.PickMode == 'network':
        #            self.SetPickMode('image')

    def CheckMenu(self):
        self.vmtkRenderer.RemoveKeyBinding('space')
        self.vmtkRenderer.RemoveKeyBinding('c')
        self.vmtkRenderer.RemoveKeyBinding('u')
        self.vmtkRenderer.RemoveKeyBinding('+')
        self.vmtkRenderer.RemoveKeyBinding('-')
        self.vmtkRenderer.RemoveKeyBinding('=')
        self.vmtkRenderer.RemoveKeyBinding('Return')

    def ShowLabelCallback(self, obj):
        self.ToggleLabels()

    def AddCallback(self, obj):
        self.InputInfo('Switched to add mode.\nCtrl + left click to add tubes.')
        #self.PrintLog('Add mode')
        self.OperationMode = 'add'
        self.InitializeActiveSegment()
        self.vmtkRenderer.AddKeyBinding('space','Toggle image/tube interaction',self.SpaceCallback, '2')
        self.vmtkRenderer.AddKeyBinding('c','Cancel',self.CancelCallback,'2')
        self.vmtkRenderer.AddKeyBinding('u','Undo',self.UndoCallback,'2')
        self.vmtkRenderer.AddKeyBinding('+','Increase radius',self.PlusCallback,'2')
        self.vmtkRenderer.AddKeyBinding('=','Increase radius',self.PlusCallback,'2')
        self.vmtkRenderer.AddKeyBinding('-','Decrease radius',self.MinusCallback,'2')
        self.vmtkRenderer.AddKeyBinding('Return','Accept tube',self.ReturnAddCallback,'2')
        self.UpdateAndRender()

    def DeleteCallback(self, obj):
        self.InputInfo('Switched to delete mode.\nCtrl + left click to delete tubes.')
        #self.PrintLog('Delete mode')
        self.OperationMode = 'delete'
        self.InitializeActiveSegment()
        self.CheckMenu()
        if self.PickMode == 'image':
            self.SetPickMode('network')
            self.InitializeActiveSegment()
        self.UpdateAndRender()

    def SplitCallback(self, obj):
        self.InputInfo('Switched to split mode.\nCtrl + left click to split tubes.')
        #self.PrintLog('Split mode')
        self.OperationMode = 'split'
        self.InitializeActiveSegment()
        self.CheckMenu()
        if self.PickMode == 'image':
            self.SetPickMode('network')
            self.InitializeActiveSegment()
        self.UpdateAndRender()

    def LabelCallback(self, obj):
        #self.PrintLog('Label mode')
        self.InputInfo('Switched to label mode.\nCtrl + left click to add label.')
        self.OperationMode = 'label'
        self.InitializeActiveSegment()
        self.CheckMenu()
        if self.PickMode == 'image':
            self.SetPickMode('network')
            self.InitializeActiveSegment()
        self.UpdateAndRender()

    def MergeCallback(self, obj):
        self.InputInfo('Switched to merge mode.\nCtrl + left click to select two tubes to merge.')
        #self.PrintLog('Merge mode')
        self.OperationMode = 'merge'
        self.InitializeActiveSegment()
        self.CheckMenu()
        if self.PickMode == 'image':
            self.SetPickMode('network')
            self.InitializeActiveSegment()
            self.CellIdsToMerge = []
        self.vmtkRenderer.AddKeyBinding('Return','Accept merge',self.ReturnCallback,'2')
        self.UpdateAndRender()

    def SpaceCallback(self, obj):
        self.TogglePickMode()
        self.TogglePlaneWidget(self.PlaneWidgetX)
        self.TogglePlaneWidget(self.PlaneWidgetY)
        self.TogglePlaneWidget(self.PlaneWidgetZ)
        self.InputInfo('Ctrl + left click to add seeds.')

    def CancelCallback(self, obj):
        self.InitializeActiveSegment()

    def UndoCallback(self, obj):
        numberOfSeeds = self.ActiveSegmentSeedsPoints.GetNumberOfPoints()
        if numberOfSeeds > 0:
            self.ActiveSegmentSeedsPoints.SetNumberOfPoints(numberOfSeeds-1)
            self.ActiveSegmentSeedsRadiusArray.SetNumberOfValues(numberOfSeeds-1)
            self.ActiveSegmentSeeds.Modified()
            self.AttachedCellIds[1] = -1
            self.AttachedSubIds[1] = -1
            self.AttachedPCoords[1] = 0.0
        else:
            self.AttachedCellIds[0] = -1
            self.AttachedSubIds[0] = -1
            self.AttachedPCoords[0] = 0.0
        numberOfPoints = self.ActiveSegmentPoints.GetNumberOfPoints()
        if numberOfPoints > 2:
            self.ActiveSegmentPoints.SetNumberOfPoints(numberOfPoints-1)
            self.ActiveSegmentRadiusArray.SetNumberOfValues(numberOfPoints-1)
            self.ActiveSegmentCellArray.Initialize()
            self.ActiveSegmentCellArray.InsertNextCell(numberOfPoints-1)
            for i in range(numberOfPoints-1):
                self.ActiveSegmentCellArray.InsertCellPoint(i)
            self.ActiveSegment.Modified()
        else:
            self.ActiveSegmentPoints.SetNumberOfPoints(0)
            self.ActiveSegmentCellArray.Initialize()
            self.ActiveSegmentRadiusArray.SetNumberOfValues(0)
            self.ActiveSegment.Modified()
        self.UpdateAndRender()

    def PlusCallback(self, obj):
        numberOfSeeds = self.ActiveSegmentSeedsPoints.GetNumberOfPoints()
        if numberOfSeeds > 0:
            radius = self.ActiveSegmentSeedsRadiusArray.GetValue(numberOfSeeds-1)
            radius += 0.1
            self.ActiveSegmentSeedsRadiusArray.SetValue(numberOfSeeds-1,radius)
            self.CurrentRadius = radius
            self.ActiveSegmentSeeds.Modified()
        numberOfPoints = self.ActiveSegmentPoints.GetNumberOfPoints()
        if numberOfPoints > 0:
            radius = self.ActiveSegmentRadiusArray.GetValue(numberOfPoints-1)
            radius += 0.1
            self.ActiveSegmentRadiusArray.SetValue(numberOfPoints-1,radius)
            self.ActiveSegment.Modified()
        self.UpdateAndRender()

    def MinusCallback(self, obj):
        numberOfSeeds = self.ActiveSegmentSeedsPoints.GetNumberOfPoints()
        if numberOfSeeds > 0:
            radius = self.ActiveSegmentSeedsRadiusArray.GetValue(numberOfSeeds-1)
            radius -= 0.1
            if radius > 0.0:
                self.ActiveSegmentSeedsRadiusArray.SetValue(numberOfSeeds-1,radius)
                self.CurrentRadius = radius
                self.ActiveSegmentSeeds.Modified()
        numberOfPoints = self.ActiveSegmentPoints.GetNumberOfPoints()
        if numberOfPoints > 0:
            radius = self.ActiveSegmentRadiusArray.GetValue(numberOfPoints-1)
            radius -= 0.1
            if radius > 0.0:
                self.ActiveSegmentRadiusArray.SetValue(numberOfPoints-1,radius)
                self.ActiveSegment.Modified()
        self.UpdateAndRender()

    def ReturnAddCallback(self, obj):
        attachedPointId0 = -1
        attachedPointId1 = -1
        cellIdsToRemove = []
        if self.AttachedCellIds[0] != -1:
            attachedPointId0, hasSplit = self.SplitCellNoRemove(self.AttachedCellIds[0],self.AttachedSubIds[0],self.AttachedPCoords[0])
            if hasSplit:
                cellIdsToRemove.append(self.AttachedCellIds[0])
        if self.AttachedCellIds[1] != -1:
            attachedPointId1, hasSplit = self.SplitCellNoRemove(self.AttachedCellIds[1],self.AttachedSubIds[1],self.AttachedPCoords[1])
            if hasSplit:
                cellIdsToRemove.append(self.AttachedCellIds[1])
        if cellIdsToRemove:
            self.RemoveCells(cellIdsToRemove)
        segment = self.ActiveSegmentActor.GetMapper().GetInput()
        if self.UseActiveTubes:
            self.RunActiveTube(segment)
        segmentRadiusArray = segment.GetPointData().GetArray(self.RadiusArrayName)
        numberOfSegmentPoints = segment.GetNumberOfPoints()
        networkPoints = self.Network.GetPoints()
        networkCellArray = self.Network.GetLines()
        cellId = networkCellArray.InsertNextCell(numberOfSegmentPoints)
        for i in range(numberOfSegmentPoints):
            id = -1
            if i == 0 and attachedPointId0 != -1:
                id = attachedPointId0
            elif i == numberOfSegmentPoints-1 and attachedPointId1 != -1:
                id = attachedPointId1
            else:
                id = networkPoints.InsertNextPoint(segment.GetPoint(i))
                self.NetworkRadiusArray.InsertTuple1(id,segmentRadiusArray.GetTuple1(i))
            networkCellArray.InsertCellPoint(id)
        self.NetworkLabelsArray.InsertValue(cellId,'')
        self.Network.BuildCells()
        self.Network.Modified()
        self.NetworkTube.Modified()
        self.UpdateLabels()
        #if self.Image and self.PickMode == 'network':
        #    self.SetPickMode('image')
        self.InitializeSelection()
        self.InitializeActiveSegment()

    def ReturnCallback(self, obj):
        numberOfActiveCells = self.ActiveSegment.GetNumberOfCells()
        if numberOfActiveCells != 2 or len(self.CellIdsToMerge) != 2:
            return
        cell0ToMerge = self.Network.GetCell(self.CellIdsToMerge[0])
        cell0PointIds = vtk.vtkIdList()
        cell0PointIds.DeepCopy(cell0ToMerge.GetPointIds())
        numberOfCell0Points = cell0PointIds.GetNumberOfIds()
        cell1ToMerge = self.Network.GetCell(self.CellIdsToMerge[1])
        cell1PointIds = vtk.vtkIdList()
        cell1PointIds.DeepCopy(cell1ToMerge.GetPointIds())
        numberOfCell1Points = cell1PointIds.GetNumberOfIds()
        reverse = [False,False]
        if cell0PointIds.GetId(numberOfCell0Points-1) == cell1PointIds.GetId(0):
            reverse = [False,False]
        elif cell0PointIds.GetId(numberOfCell0Points-1) == cell1PointIds.GetId(numberOfCell1Points-1):
            reverse = [False,True]
        elif cell0PointIds.GetId(0) == cell1PointIds.GetId(numberOfCell1Points-1):
            reverse = [True,True]
        elif cell0PointIds.GetId(0) == cell1PointIds.GetId(0):
            reverse = [True,False]
        else:
            self.PrintLog('Error: trying to merge non-adjacent segments.')
            return
        mergedLabel = self.NetworkLabelsArray.GetValue(self.CellIdsToMerge[0])
        self.RemoveCells(self.CellIdsToMerge)
        networkCellArray = self.Network.GetLines()
        cellId = networkCellArray.InsertNextCell(numberOfCell0Points+numberOfCell1Points-1)
        for i in range(numberOfCell0Points):
            loc = i
            if reverse[0]:
                loc = numberOfCell0Points - 1 - i
            networkCellArray.InsertCellPoint(cell0PointIds.GetId(loc))
        for i in range(1,numberOfCell1Points):
            loc = i
            if reverse[1]:
                loc = numberOfCell1Points - 1 - i
            networkCellArray.InsertCellPoint(cell1PointIds.GetId(loc))
        self.NetworkLabelsArray.InsertValue(cellId,mergedLabel)
        self.Network.SetLines(networkCellArray)
        self.Network.BuildCells()
        self.Network.Modified()
        self.NetworkTube.Modified()
        self.UpdateLabels()
        self.InitializeActiveSegment()
        self.CellIdsToMerge = []

    def RemoveCell(self,cellId):
        self.RemoveCells([cellId])

    def RemoveCells(self,cellIds):
        networkCellArray = vtk.vtkCellArray()
        networkLabelsArray = vtk.vtkStringArray()
        numberOfCells = self.Network.GetNumberOfCells()
        for i in range(numberOfCells):
            if i in cellIds:
                continue
            networkCellArray.InsertNextCell(self.Network.GetCell(i))
            networkLabelsArray.InsertNextValue(self.NetworkLabelsArray.GetValue(i))
        self.Network.SetLines(networkCellArray)
        self.NetworkLabelsArray.DeepCopy(networkLabelsArray)
        self.Network.BuildCells()
        self.Network.Modified()

    def SplitCell(self,cellId,subId,pcoord):
        splitPointId, hasSplit = self.SplitCellNoRemove(cellId,subId,pcoord)
        if hasSplit:
            self.RemoveCell(cellId)
        return splitPointId, hasSplit

    def SplitCellNoRemove(self,cellId,subId,pcoord):
        splitId = subId
        if pcoord > 0.5:
            splitId = subId+1
        cell = self.Network.GetCell(cellId)
        label = self.NetworkLabelsArray.GetValue(cellId)
        numberOfCellPoints = cell.GetNumberOfPoints()
        cellPointIds = cell.GetPointIds()
        splitPointId = cellPointIds.GetId(splitId)
        if splitId == 0 or splitId == numberOfCellPoints-1:
            return splitPointId, False
        lines = self.Network.GetLines()
        newCellId1 = lines.InsertNextCell(splitId+1)
        for i in range(splitId+1):
            lines.InsertCellPoint(cellPointIds.GetId(i))
        newCellId2 = lines.InsertNextCell(numberOfCellPoints-splitId)
        for i in range(splitId,numberOfCellPoints):
            lines.InsertCellPoint(cellPointIds.GetId(i))
        newLabel1 = ''
        newLabel2 = ''
        if label:
            newLabel1 = label + "1"
            newLabel2 = label + "2"
        self.NetworkLabelsArray.InsertValue(newCellId1,newLabel1)
        self.NetworkLabelsArray.InsertValue(newCellId2,newLabel2)
        self.Network.BuildCells()
        return splitPointId, True

    def LeftButtonPressCallback(self,obj,event):
        if self.PickMode != 'network':
            return
        if self.vmtkRenderer.RenderWindowInteractor.GetControlKey() == 0:
            return
        if self.Network.GetNumberOfCells() == 0:
            return
        cellId = self.SelectedCellId
        if cellId == -1:
            return
        subId = self.SelectedSubId
        pcoords = self.SelectedPCoords
        cell = self.Network.GetCell(cellId)
        if pcoords < 0.5:
            pointId = cell.GetPointIds().GetId(subId)
        else:
            pointId = cell.GetPointIds().GetId(subId+1)
        point = self.Network.GetPoint(pointId)
        radius = self.NetworkRadiusArray.GetValue(pointId)
        if self.OperationMode == 'add':
            if radius == 0.0:
                radius = 1.0
            if self.ActiveSegmentSeeds.GetNumberOfPoints() == 0:
                self.ActiveSegmentSeedsPoints.InsertNextPoint(point)
                self.ActiveSegmentSeedsRadiusArray.InsertNextValue(radius)
                self.ActiveSegmentSeeds.Modified()
                self.AttachedCellIds[0] = cellId
                self.AttachedSubIds[0] = subId
                self.AttachedPCoords[0] = pcoords
                self.Render()
                return
            if self.AttachedCellIds[1] != -1:
                return
            self.AttachedCellIds[1] = cellId
            self.AttachedSubIds[1] = subId
            self.AttachedPCoords[1] = pcoords
            if self.ActiveSegmentPoints.GetNumberOfPoints() == 0:
                self.ActiveSegmentPoints.InsertNextPoint(self.ActiveSegmentSeedsPoints.GetPoint(0))
                self.ActiveSegmentRadiusArray.InsertNextValue(self.ActiveSegmentSeedsRadiusArray.GetValue(0))
            self.ActiveSegmentSeedsPoints.InsertNextPoint(point)
            self.ActiveSegmentSeedsRadiusArray.InsertNextValue(radius)
            self.ActiveSegmentSeeds.Modified()
            self.ActiveSegmentPoints.InsertNextPoint(point)
            self.ActiveSegmentRadiusArray.InsertNextValue(radius)
            self.ActiveSegmentCellArray.Initialize()
            numberOfPoints = self.ActiveSegmentPoints.GetNumberOfPoints()
            self.ActiveSegmentCellArray.InsertNextCell(numberOfPoints)
            for i in range(numberOfPoints):
                self.ActiveSegmentCellArray.InsertCellPoint(i)
            self.ActiveSegment.Modified()
            self.Render()
        elif self.OperationMode == 'delete':
            self.InitializeSelection()
            self.RemoveCell(cellId)
            self.NetworkTube.Modified()
            self.UpdateLabels()
        elif self.OperationMode == 'split':
            numberOfCellPoints = cell.GetNumberOfPoints()
            self.SplitCell(cellId,subId,pcoords)
            self.NetworkTube.Modified()
            self.UpdateLabels()
        elif self.OperationMode == 'label':
            self.LabelCellId = cellId
            label = self.NetworkLabelsArray.GetValue(cellId)
            if label:
                self.PrintLog("Current label: %s" % label)
            self.vmtkRenderer.PromptAsync("Please input new label: ",self.QueryLabelCallback)
        elif self.OperationMode == 'merge':
            if self.ActiveSegment.GetNumberOfCells() > 1:
                self.InitializeActiveSegment()
                self.CellIdsToMerge = []
            self.CellIdsToMerge.append(cellId)
            numberOfCellPoints = cell.GetNumberOfPoints()
            activePointIds = []
            for i in range(numberOfCellPoints):
                pointId = cell.GetPointId(i)
                point = self.Network.GetPoint(pointId)
                radius = self.NetworkRadiusArray.GetTuple1(pointId)
                activePointId = self.ActiveSegmentPoints.InsertNextPoint(point)
                self.ActiveSegmentRadiusArray.InsertNextValue(radius)
                activePointIds.append(activePointId)
            self.ActiveSegmentCellArray.InsertNextCell(numberOfCellPoints)
            for i in range(numberOfCellPoints):
                self.ActiveSegmentCellArray.InsertCellPoint(activePointIds[i])
            self.ActiveSegment.Modified()
            self.NetworkTube.Modified()
            self.Render()

    def QueryLabelCallback(self,newLabel):
        self.NetworkLabelsArray.SetValue(self.LabelCellId,newLabel)
        self.LabelCellId = None
        self.InitializeSelection()
        self.Network.Modified()
        self.UpdateLabels()

    def InitializeSelection(self):
        self.SelectionPoints.SetNumberOfPoints(0)
        self.SelectionRadiusArray.SetNumberOfValues(0)
        self.SelectedCellId = -1
        self.SelectedSubId = -1
        self.SelectedPCoords = 0.0
        self.Selection.Modified()
        self.Render()

    def InitializeActiveSegment(self):
        self.ActiveSegmentSeedsPoints.SetNumberOfPoints(0)
        self.ActiveSegmentSeedsRadiusArray.SetNumberOfValues(0)
        self.ActiveSegmentSeeds.Modified()
        self.ActiveSegmentPoints.SetNumberOfPoints(0)
        self.ActiveSegmentCellArray.Initialize()
        self.ActiveSegmentCellArray.SetNumberOfCells(0)
        self.ActiveSegmentRadiusArray.SetNumberOfValues(0)
        self.ActiveSegment.Modified()
        self.AttachedCellIds = [-1,-1]
        self.AttachedSubIds = [-1,-1]
        self.AttachedPCoords = [0.0,0.0]
        self.Render()

    def MouseMoveCallback(self,obj,event):
        if self.PickMode != 'network':
            return
        if self.vmtkRenderer.RenderWindowInteractor.GetControlKey() == 0:
            if self.SelectedCellId != -1:
                self.InitializeSelection()
            return
        if self.Network.GetNumberOfCells() == 0:
            return
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        result = self.CellPicker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        if result == 0:
            return
        cellId = self.CellPicker.GetCellId()
        cell = self.Network.GetCell(cellId)
        subId = self.CellPicker.GetSubId()
        pcoords = self.CellPicker.GetPCoords()[0]
        if pcoords < 0.5:
            pointId = cell.GetPointIds().GetId(subId)
        else:
            pointId = cell.GetPointIds().GetId(subId+1)
        if self.vmtkRenderer.RenderWindowInteractor.GetShiftKey() == 1:
            if subId < cell.GetNumberOfPoints()/2:
                pointId = cell.GetPointIds().GetId(0)
            else:
                pointId = cell.GetPointIds().GetId(cell.GetNumberOfPoints()-1)
        point = self.Network.GetPoint(pointId)
        radius = self.NetworkRadiusArray.GetValue(pointId)
        if radius == 0.0:
            radius = 1.0
        self.SelectionPoints.InsertPoint(0,point)
        self.SelectionRadiusArray.InsertValue(0,radius)
        self.SelectedCellId = cellId
        self.SelectedSubId = subId
        self.SelectedPCoords = pcoords
        self.Selection.Modified()
        self.Render()

    def PlaneStartInteractionCallback(self,obj,event):
        if self.PickMode != 'image':
            return
        if self.OperationMode != 'add':
            return
        if self.vmtkRenderer.RenderWindowInteractor.GetControlKey() == 0:
            return
        point = obj.GetCurrentCursorPosition()
        radius = self.CurrentRadius
        if self.ActiveSegmentSeeds.GetNumberOfPoints() == 0:
            self.ActiveSegmentSeedsPoints.InsertNextPoint(point)
            self.ActiveSegmentSeedsRadiusArray.InsertNextValue(radius)
            self.ActiveSegmentSeeds.Modified()
            self.Render()
            return
        if self.AttachedCellIds[1] != -1:
            return
        if self.ActiveSegmentPoints.GetNumberOfPoints() == 0:
            self.ActiveSegmentPoints.InsertNextPoint(self.ActiveSegmentSeedsPoints.GetPoint(0))
            self.ActiveSegmentRadiusArray.InsertNextValue(self.ActiveSegmentSeedsRadiusArray.GetValue(0))
        self.ActiveSegmentSeedsPoints.InsertNextPoint(point)
        self.ActiveSegmentSeedsRadiusArray.InsertNextValue(radius)
        self.ActiveSegmentSeeds.Modified()
        self.ActiveSegmentPoints.InsertNextPoint(point)
        self.ActiveSegmentRadiusArray.InsertNextValue(radius)
        self.ActiveSegmentCellArray.Initialize()
        numberOfPoints = self.ActiveSegmentPoints.GetNumberOfPoints()
        self.ActiveSegmentCellArray.InsertNextCell(numberOfPoints)
        for i in range(numberOfPoints):
            self.ActiveSegmentCellArray.InsertCellPoint(i)
        self.ActiveSegment.Modified()
        self.Render()

    def FirstRender(self):
        self.vmtkRenderer.Render()

    def Render(self):
        self.vmtkRenderer.RenderWindow.Render()

    def UpdateAndRender(self):
        self.vmtkRenderer.Render(interactive=0)

    def RunActiveTube(self,segment):
        activeTubes = vmtkactivetubes.vmtkActiveTubes()
        activeTubes.Centerline = segment
        activeTubes.Image = self.FeatureImage
        activeTubes.NumberOfIterations = self.NumberOfIterations
        activeTubes.PotentialWeight = self.PotentialWeight
        activeTubes.StiffnessWeight = self.StiffnessWeight
        activeTubes.RadiusArrayName = self.RadiusArrayName
        activeTubes.Execute()
        segment.DeepCopy(activeTubes.Centerline)

    def Execute(self):

        if not self.Network:
            self.Network = vtk.vtkPolyData()
            networkPoints = vtk.vtkPoints()
            networkLines = vtk.vtkCellArray()
            radiusArray = vtk.vtkDoubleArray()
            radiusArray.SetName(self.RadiusArrayName)
            self.Network.SetPoints(networkPoints)
            self.Network.SetLines(networkLines)
            self.Network.GetPointData().AddArray(radiusArray)

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.ExitAfterTextInputMode = False
        self.vmtkRenderer.RegisterScript(self)

        if self.Image and (not self.PlaneWidgetX or not self.PlaneWidgetY or not self.PlaneWidgetZ):
            imageViewer = vmtkimageviewer.vmtkImageViewer()
            imageViewer.Image = self.Image
            imageViewer.vmtkRenderer = self.vmtkRenderer
            imageViewer.Display = 0
            imageViewer.Execute()
            self.PlaneWidgetX = imageViewer.PlaneWidgetX
            self.PlaneWidgetY = imageViewer.PlaneWidgetY
            self.PlaneWidgetZ = imageViewer.PlaneWidgetZ

        if self.Image:
            spacing = self.Image.GetSpacing()
            self.CurrentRadius = min(spacing)

        if self.UseActiveTubes and not self.FeatureImage:
            imageFeatures = vmtkimagefeatures.vmtkImageFeatures()
            imageFeatures.Image = self.Image
            imageFeatures.FeatureImageType = 'vtkgradient'
            imageFeatures.Execute()
            self.FeatureImage = imageFeatures.FeatureImage

        self.NetworkRadiusArray = self.Network.GetPointData().GetArray(self.RadiusArrayName)

        self.Network.GetPointData().SetActiveScalars(self.RadiusArrayName)

        networkMapper = vtk.vtkPolyDataMapper()
        networkMapper.SetInputData(self.Network)
        networkMapper.SetScalarModeToUseCellData()

        self.NetworkActor = vtk.vtkActor()
        self.NetworkActor.SetMapper(networkMapper)
        self.vmtkRenderer.Renderer.AddActor(self.NetworkActor)

        self.NetworkTube = vtk.vtkTubeFilter()
        self.NetworkTube.SetInputData(self.Network)
        self.NetworkTube.SetVaryRadiusToVaryRadiusByAbsoluteScalar()
        self.NetworkTube.SetNumberOfSides(20)
        networkTubeMapper = vtk.vtkPolyDataMapper()
        networkTubeMapper.SetInputConnection(self.NetworkTube.GetOutputPort())
        networkTubeMapper.ScalarVisibilityOff()
        networkTubeActor = vtk.vtkActor()
        networkTubeActor.SetMapper(networkTubeMapper)
        networkTubeActor.PickableOff()
        networkTubeActor.GetProperty().SetOpacity(0.2)
        self.vmtkRenderer.Renderer.AddActor(networkTubeActor)

        self.Selection = vtk.vtkPolyData()
        self.SelectionPoints = vtk.vtkPoints()
        self.SelectionRadiusArray = vtk.vtkDoubleArray()
        self.SelectionRadiusArray.SetName(self.RadiusArrayName)
        self.Selection.SetPoints(self.SelectionPoints)
        self.Selection.GetPointData().AddArray(self.SelectionRadiusArray)
        self.Selection.GetPointData().SetActiveScalars(self.RadiusArrayName)

        glyphs = vtk.vtkGlyph3D()
        glyphSource = vtk.vtkSphereSource()
        glyphSource.SetRadius(1.0)
        glyphSource.SetThetaResolution(20)
        glyphSource.SetPhiResolution(20)
        glyphs.SetInputData(self.Selection)
        glyphs.SetSourceConnection(glyphSource.GetOutputPort())
        glyphs.SetScaleModeToScaleByScalar()
        glyphs.SetScaleFactor(1.0)

        selectionMapper = vtk.vtkPolyDataMapper()
        selectionMapper.SetInputConnection(glyphs.GetOutputPort())

        self.SelectionActor = vtk.vtkActor()
        self.SelectionActor.SetMapper(selectionMapper)
        self.SelectionActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.SelectionActor.GetProperty().SetOpacity(0.5)
        self.SelectionActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.SelectionActor)

        self.ActiveSegmentSeeds = vtk.vtkPolyData()
        self.ActiveSegmentSeedsPoints = vtk.vtkPoints()
        self.ActiveSegmentSeedsRadiusArray = vtk.vtkDoubleArray()
        self.ActiveSegmentSeedsRadiusArray.SetName(self.RadiusArrayName)
        self.ActiveSegmentSeeds.SetPoints(self.ActiveSegmentSeedsPoints)
        self.ActiveSegmentSeeds.GetPointData().AddArray(self.ActiveSegmentSeedsRadiusArray)
        self.ActiveSegmentSeeds.GetPointData().SetActiveScalars(self.RadiusArrayName)

        activeSegmentSeedsGlyphs = vtk.vtkGlyph3D()
        activeSegmentSeedsGlyphSource = vtk.vtkSphereSource()
        activeSegmentSeedsGlyphSource.SetRadius(1.0)
        activeSegmentSeedsGlyphSource.SetThetaResolution(20)
        activeSegmentSeedsGlyphSource.SetPhiResolution(20)
        activeSegmentSeedsGlyphs.SetInputData(self.ActiveSegmentSeeds)
        activeSegmentSeedsGlyphs.SetSourceConnection(activeSegmentSeedsGlyphSource.GetOutputPort())
        activeSegmentSeedsGlyphs.SetScaleModeToScaleByScalar()
        activeSegmentSeedsGlyphs.SetScaleFactor(1.0)

        activeSegmentSeedsMapper = vtk.vtkPolyDataMapper()
        activeSegmentSeedsMapper.SetInputConnection(activeSegmentSeedsGlyphs.GetOutputPort())
        activeSegmentSeedsMapper.ScalarVisibilityOff()

        self.ActiveSegmentSeedsActor = vtk.vtkActor()
        self.ActiveSegmentSeedsActor.SetMapper(activeSegmentSeedsMapper)
        self.ActiveSegmentSeedsActor.GetProperty().SetColor(1.0,0.0,0.0)
        self.ActiveSegmentSeedsActor.GetProperty().SetOpacity(0.5)
        self.ActiveSegmentSeedsActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.ActiveSegmentSeedsActor)

        self.ActiveSegment = vtk.vtkPolyData()
        self.ActiveSegmentPoints = vtk.vtkPoints()
        self.ActiveSegmentCellArray = vtk.vtkCellArray()
        self.ActiveSegmentRadiusArray = vtk.vtkDoubleArray()
        self.ActiveSegmentRadiusArray.SetName(self.RadiusArrayName)
        self.ActiveSegment.SetPoints(self.ActiveSegmentPoints)
        self.ActiveSegment.SetLines(self.ActiveSegmentCellArray)
        self.ActiveSegment.GetPointData().AddArray(self.ActiveSegmentRadiusArray)
        self.ActiveSegment.GetPointData().SetActiveScalars(self.RadiusArrayName)

        activeSegmentMapper = vtk.vtkPolyDataMapper()
        activeSegmentMapper.ScalarVisibilityOff()
        if self.SplineInterpolation and self.Image != None:
            splineFilter = vtk.vtkSplineFilter()
            splineFilter.SetInputData(self.ActiveSegment)
            splineFilter.SetSubdivideToLength()
            splineFilter.SetLength(2.0*min(self.Image.GetSpacing()))
            activeSegmentMapper.SetInputConnection(splineFilter.GetOutputPort())
        else:
            activeSegmentMapper.SetInputData(self.ActiveSegment)

        self.ActiveSegmentActor = vtk.vtkActor()
        self.ActiveSegmentActor.SetMapper(activeSegmentMapper)
        self.ActiveSegmentActor.GetProperty().SetColor(1.0,1.0,1.0)
        self.ActiveSegmentActor.GetProperty().SetLineWidth(3.0)
        self.ActiveSegmentActor.PickableOff()
        self.vmtkRenderer.Renderer.AddActor(self.ActiveSegmentActor)

        activeTube = vtk.vtkTubeFilter()
        activeTube.SetInputData(activeSegmentMapper.GetInput())
        activeTube.SetVaryRadiusToVaryRadiusByAbsoluteScalar()
        activeTube.SetNumberOfSides(20)
        activeTubeMapper = vtk.vtkPolyDataMapper()
        activeTubeMapper.SetInputConnection(activeTube.GetOutputPort())
        activeTubeMapper.ScalarVisibilityOff()
        activeTubeActor = vtk.vtkActor()
        activeTubeActor.SetMapper(activeTubeMapper)
        activeTubeActor.PickableOff()
        activeTubeActor.GetProperty().SetOpacity(0.6)
        self.vmtkRenderer.Renderer.AddActor(activeTubeActor)

        self.NetworkLabelsArray = vtk.vtkStringArray.SafeDownCast(self.Network.GetCellData().GetAbstractArray(self.LabelsArrayName))
        if not self.NetworkLabelsArray:
            self.NetworkLabelsArray = vtk.vtkStringArray()
            self.NetworkLabelsArray.SetName(self.LabelsArrayName)
            self.NetworkLabelsArray.SetNumberOfValues(self.Network.GetNumberOfCells())
            for i in range(self.Network.GetNumberOfCells()):
                self.NetworkLabelsArray.SetValue(i,'')
            self.Network.GetCellData().AddArray(self.NetworkLabelsArray)
        self.CellCenters = vtk.vtkCellCenters()
        self.CellCenters.SetInputData(self.Network)
        self.CellCenters.VertexCellsOff()
        self.CellCenters.Update()

        labeledMapper = vtk.vtkLabeledDataMapper()
        labeledMapper.SetInputConnection(self.CellCenters.GetOutputPort())
        labeledMapper.SetLabelModeToLabelFieldData()
        labeledMapper.SetFieldDataName(self.LabelsArrayName)
        labeledMapper.GetLabelTextProperty().SetFontFamilyToArial()
        labeledMapper.GetLabelTextProperty().BoldOff()
        labeledMapper.GetLabelTextProperty().ItalicOff()
        labeledMapper.GetLabelTextProperty().ShadowOff()

        self.LabelsActor = vtk.vtkActor2D()
        self.LabelsActor.SetMapper(labeledMapper)
        self.LabelsActor.VisibilityOff()
        self.vmtkRenderer.Renderer.AddActor(self.LabelsActor)

        self.CellPicker = vtk.vtkCellPicker()
        self.CellPicker.SetTolerance(1E-2)
        self.CellPicker.InitializePickList()
        self.CellPicker.AddPickList(self.NetworkActor)
        self.CellPicker.PickFromListOn()

        self.vmtkRenderer.AddKeyBinding('a','Add mode.',self.AddCallback)
        self.vmtkRenderer.AddKeyBinding('d','Delete mode.',self.DeleteCallback)
        self.vmtkRenderer.AddKeyBinding('m','Merge mode.',self.MergeCallback)
        self.vmtkRenderer.AddKeyBinding('s','Split mode.',self.SplitCallback)
        self.vmtkRenderer.AddKeyBinding('l','Label mode.',self.LabelCallback)
        self.vmtkRenderer.AddKeyBinding('Tab','Show labels.',self.ShowLabelCallback)
        self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyReleaseEvent", self.KeyReleaseCallback)
        self.vmtkRenderer.RenderWindowInteractor.AddObserver("LeftButtonPressEvent", self.LeftButtonPressCallback)
        self.vmtkRenderer.RenderWindowInteractor.AddObserver("MouseMoveEvent", self.MouseMoveCallback)

        if self.PlaneWidgetX:
            self.PlaneWidgetX.UseContinuousCursorOn()
            self.PlaneWidgetX.AddObserver("StartInteractionEvent", self.PlaneStartInteractionCallback)
        if self.PlaneWidgetY:
            self.PlaneWidgetY.UseContinuousCursorOn()
            self.PlaneWidgetY.AddObserver("StartInteractionEvent", self.PlaneStartInteractionCallback)
        if self.PlaneWidgetZ:
            self.PlaneWidgetZ.UseContinuousCursorOn()
            self.PlaneWidgetZ.AddObserver("StartInteractionEvent", self.PlaneStartInteractionCallback)

        self.FirstRender()

        self.Surface = self.NetworkTube.GetOutput()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
