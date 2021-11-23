#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceconnectivityselector.py,v $
## Language:  Python
## Date:      $Date: 2018/04/03 12:49:59 $
## Version:   $Revision: 0.1 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkPickPointSeedSelector(object):

    def __init__(self):
        self.PrintError = None
        self.PrintLog = None
        self.InputText = None
        self.OutputText = None
        self.InputInfo = None
        self._OutputPolyDataList = []

        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Script = None
        self.CurrentPickedActor = None
        self.CurrentPickedPolyData = None
        self.PastPickedActorList = []

    def UndoCallback(self, obj):
        # The execution order is extremly important in this method. Do not change if possible.
        self.vmtkRenderer.RenderWindow.Render()
        # First remove the vtkPolyDataObject from the list where it is being stored.
        # by default we remove the last element selected first
        self._OutputPolyDataList.pop()
        # the actor instance is stored within the PastPickedActorList object. We tell the renderer
        # to remove the actor from the scene, then remove that actor from the list.
        self.vmtkRenderer.Renderer.RemoveActor(self.PastPickedActorList[-1])
        self.PastPickedActorList.pop()
        # update the scene in the render window
        self.vmtkRenderer.RenderWindow.Render()
        return

    def PickCallback(self, obj):
        picker = vtk.vtkPointPicker()
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        # We treat result as a boolean value (which can also be nonetype if a selection outside the render windw is made)
        # If the value of result is False or None, then an invalid actor was selected in the scene.
        if result is None:
            return
        elif result == 0:
            return

        # find the value of "RegionId" stored in the vtkPolyData PointData array. This is a unique number
        # which is generated for each connected surface point by the allRegionConnectivity
        # vtkPolyDataConnectivityFilter function
        pickedPointId = picker.GetPointId()
        self.CurrentPickedActor = picker.GetActor()
        self.CurrentPickedPolyData = self.CurrentPickedActor.GetMapper().GetInputAsDataSet()
        regionId = self.CurrentPickedPolyData.GetPointData().GetArray('RegionId').GetValue(pickedPointId)

        # extract only the desired region Id from the dataset
        isolatedFilter = vtk.vtkPolyDataConnectivityFilter()
        isolatedFilter.SetInputData(self.CurrentPickedPolyData)
        isolatedFilter.SetExtractionModeToSpecifiedRegions()
        isolatedFilter.AddSpecifiedRegion(regionId)

        # remove all points not in the extracted cells
        cleanedFilter = vtk.vtkCleanPolyData()
        cleanedFilter.SetInputConnection(isolatedFilter.GetOutputPort())
        cleanedFilter.Update()

        outputSurface = cleanedFilter.GetOutput()

        # create a new mapper and actor for this extracted surface object
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(outputSurface)
        mapper.ScalarVisibilityOff()
        mapper.Update()

        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        actor.GetProperty().SetColor(1.0, 0.0, 0.0) # red color
        actor.GetProperty().SetDiffuse(1.0)
        actor.GetProperty().SetSpecular(0.2)
        actor.GetProperty().SetRepresentationToSurface()
        actor.GetProperty().EdgeVisibilityOff()

        # place the extracted surface object's actor instance in the necessary render windows
        # and lists to log it's selection
        self.PastPickedActorList.append(actor)
        self._OutputPolyDataList.append(outputSurface)
        self.vmtkRenderer.Renderer.AddActor(actor)
        self.vmtkRenderer.RenderWindow.Render()
        return

    def Execute(self):
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self.Script)

        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('space','Add points.',self.PickCallback)

        self.InputInfo('Please position the mouse and press space to add target points, \'u\' to undo\n')

        # this is necessary to set up the async watch loop.
        any = 0
        while any == 0:
            self.vmtkRenderer.Render()
            try:
                any = self.CurrentPickedPolyData
            except AttributeError:
                any = None

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


class vmtkSurfaceConnectivitySelector(pypes.pypeScript):

    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.SeedSelector = None
        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.OutputSeedIds = []

        self.SetScriptName('vmtksurfaceconnectivityselector')
        self.SetScriptDoc('interactively select the desired surfaces from a dataset composed of disconnected components')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']])

    def Execute(self):
        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        # a check to make sure nothing has ran out of order.
        if self.SeedSelector:
            pass
        else:
            # initialize the seed selection method
            self.SeedSelector = vmtkPickPointSeedSelector()
            self.SeedSelector.vmtkRenderer = self.vmtkRenderer
            self.SeedSelector.Script = self

        # label all non-connected regions in an input surface.
        allRegionConnectivity = vtk.vtkPolyDataConnectivityFilter()
        allRegionConnectivity.SetInputData(self.Surface)
        allRegionConnectivity.SetExtractionModeToAllRegions()
        allRegionConnectivity.ColorRegionsOn()
        allRegionConnectivity.Update()

        # create the actor/mapper which renders the labeled input surface
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(allRegionConnectivity.GetOutput())
        mapper.ScalarVisibilityOff()
        mapper.Update()

        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        actor.GetProperty().SetColor(0.7, 0.7, 0.7) # standard gray color
        actor.GetProperty().SetDiffuse(0.9)
        actor.GetProperty().SetSpecular(0.2)
        actor.GetProperty().SetRepresentationToSurface()
        actor.GetProperty().EdgeVisibilityOff()

        # add this as the main actor to the scene (the first think you see)
        self.vmtkRenderer.Renderer.AddActor(actor)

        if self.SeedSelector:
            self.SeedSelector.InputInfo = self.InputInfo
            self.SeedSelector.InputText = self.InputText
            self.SeedSelector.OutputText = self.OutputText
            self.SeedSelector.PrintError = self.PrintError
            self.SeedSelector.PrintLog = self.PrintLog
            # until SeedSelector releases control of execution, the program will not
            # proceed past the next line. SeedSelector handles all user interaction and
            # modification/logging of actor properties & region Ids
            self.SeedSelector.Execute()

            # we store each extracted region as individual vtkPolyData objects within a list.
            # They are combined into one dataset below.
            polyDataAppender = vtk.vtkAppendPolyData()
            for outputSurfacePolyData in self.SeedSelector._OutputPolyDataList:
                polyDataAppender.AddInputData(outputSurfacePolyData)

            # Even though we clean the selection in the SeedSelection, this is necessary because it
            # may be possible for a user to select the same polydata object twice. If this is not cleaned,
            # a double selection may mess with further post processing scripts.
            polyDataCleaner = vtk.vtkCleanPolyData()
            polyDataCleaner.SetInputConnection(polyDataAppender.GetOutputPort())
            polyDataCleaner.Update()

            self.Surface = polyDataAppender.GetOutput()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
