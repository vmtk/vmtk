#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceconnectivityselector.py,v $
## Language:  Python
## Date:      $Date: 2018/04/03 12:49:59 $
## Version:   $Revision: 0.1 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

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

        self.PickedSeedIds = vtk.vtkIdList()
        self.PickedSeeds = vtk.vtkPolyData()
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Script = None
        

    def UndoCallback(self, obj):
        self.InitializeSeeds()
        self.PickedSeeds.Modified()
        self.vmtkRenderer.RenderWindow.Render()
        self._OutputPolyDataList.pop()
        self.PickedActor.GetProperty().SetColor(0.7, 0.7, 0.7)
        self.PickedActor.GetProperty().SetDiffuse(0.9)
        self.PickedActor.GetProperty().SetSpecular(0.2)
        return


    def PickCallback(self, obj):
        picker = vtk.vtkPropPicker()
        eventPosition = self.vmtkRenderer.RenderWindowInteractor.GetEventPosition()
        result = picker.Pick(float(eventPosition[0]),float(eventPosition[1]),0.0,self.vmtkRenderer.Renderer)
        if result is None:
            return
        elif result == 0:
            return
        self.PickedActor = picker.GetActor()
        
        self.PickedActor.GetProperty().SetColor(1.0, 0.0, 0.0)
        self.PickedActor.GetProperty().SetDiffuse(1.0)
        self.PickedActor.GetProperty().SetSpecular(0.2)
        
        self.PickedPoly = self.PickedActor.GetMapper().GetInputAsDataSet()
        self._OutputPolyDataList.append(self.PickedPoly)  
        return

        
    def InitializeSeeds(self):
        self.PickedSeedIds.Initialize()
        self.PickedSeeds.Initialize()
        seedPoints = vtk.vtkPoints()
        self.PickedSeeds.SetPoints(seedPoints)


    def Execute(self):
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self.Script)

        self.vmtkRenderer.AddKeyBinding('u','Undo.',self.UndoCallback)
        self.vmtkRenderer.AddKeyBinding('space','Add points.',self.PickCallback)

        self.InputInfo('Please position the mouse and press space to add target points, \'u\' to undo\n')

        any = 0
        while any == 0:
            self.InitializeSeeds()
            self.vmtkRenderer.Render()
            try:
                any = self.PickedPoly
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
            self.SeedSelector = vmtkPickPointSeedSelector()
            self.SeedSelector.vmtkRenderer = self.vmtkRenderer
            self.SeedSelector.Script = self
        
        connectiv = vtk.vtkPolyDataConnectivityFilter()
        connectiv.SetInputData(self.Surface)
        connectiv.SetExtractionModeToAllRegions()
        connectiv.ColorRegionsOn()
        connectiv.Update()
        numRegions = connectiv.GetNumberOfExtractedRegions()

        for surfaceId in range(numRegions):
            connectiv2 = vtk.vtkPolyDataConnectivityFilter()
            connectiv2.SetExtractionModeToSpecifiedRegions()
            connectiv2.SetInputData(connectiv.GetOutput())
            connectiv2.AddSpecifiedRegion(surfaceId)
            connectiv2.Update()
            surf = connectiv2.GetOutput()
            
            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInputData(surf)
            mapper.ScalarVisibilityOff()
            actor = vtk.vtkActor()
            actor.SetMapper(mapper)

            actor.GetProperty().SetColor(0.7, 0.7, 0.7)
            actor.GetProperty().SetDiffuse(0.9)
            actor.GetProperty().SetSpecular(0.2)
            actor.GetProperty().SetRepresentationToSurface()
            actor.GetProperty().EdgeVisibilityOff()

            self.vmtkRenderer.Renderer.AddActor(actor)

        if self.SeedSelector:
            self.SeedSelector.InputInfo = self.InputInfo
            self.SeedSelector.InputText = self.InputText
            self.SeedSelector.OutputText = self.OutputText
            self.SeedSelector.PrintError = self.PrintError
            self.SeedSelector.PrintLog = self.PrintLog
            self.SeedSelector.Execute()

            polyDataAppender = vtk.vtkAppendPolyData()
            for outputSurfacePolyData in self.SeedSelector._OutputPolyDataList:
                polyDataAppender.AddInputData(outputSurfacePolyData)
            polyDataAppender.Update()    

            self.Surface = polyDataAppender.GetOutput()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__ == '__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()