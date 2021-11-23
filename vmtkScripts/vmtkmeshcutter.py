#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshcutter.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.8 $

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


class vmtkMeshCutter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Actor = None
        self.Display = 1
        self.PlaneWidget = None
        self.ObserverId = -1
        self.Opacity = 0.05

        self.SetScriptName('vmtkmeshcutter')
        self.SetScriptDoc('slicing a mesh with a plane')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Opacity','opacity','float',0.05,'(0.0,1.0)','object opacity in the scene']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def CutMesh(self):
        cutPlane = vtk.vtkPlane()
        self.PlaneWidget.GetPlane(cutPlane)
        self.MeshCutFilter.SetCutFunction(cutPlane)
        self.MeshCutFilter.Update()
        self.Actor = vtk.vtkActor()
        mapper = vtk.vtkDataSetMapper()
        mapper.SetInputConnection(self.MeshCutFilter.GetOutputPort())
        self.Actor.SetMapper(mapper)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)

    def StartPlaneCallback(self,widget,event_string):
        pass

    def EndPlaneCallback(self,widget,event_string):
        self.CutMesh()

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        self.PlaneWidget = vtk.vtkImplicitPlaneWidget()

        if self.Actor != None:
            self.vmtkRenderer.Renderer.RemoveActor(self.Actor)

        if self.Mesh != None:

            self.MeshCutFilter = vtk.vtkCutter()
            self.MeshCutFilter.SetInputData(self.Mesh)
            cutPlane = vtk.vtkPlane()
            self.PlaneWidget.GetPlane(cutPlane)
            self.MeshCutFilter.SetCutFunction(cutPlane)
            self.MeshCutFilter.Update()
            self.Surface = self.MeshCutFilter.GetOutput()

            self.PlaneWidget.AddObserver("StartInteractionEvent",self.StartPlaneCallback)
            self.PlaneWidget.AddObserver("EndInteractionEvent",self.EndPlaneCallback)

            mapper = vtk.vtkDataSetMapper()
            mapper.SetInputData(self.Mesh)

            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)
            self.Actor.GetProperty().SetOpacity(self.Opacity)

        self.PlaneWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidget.SetPlaceFactor(1.25)
        self.PlaneWidget.DrawPlaneOn()
        self.PlaneWidget.GetPlaneProperty().SetOpacity(0.1)
        self.PlaneWidget.SetProp3D(self.Actor)
        self.PlaneWidget.PlaceWidget()
        self.PlaneWidget.SetOrigin(self.Actor.GetCenter())
        self.PlaneWidget.On()

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if self.Mesh == None and self.Display == 1:
            self.PrintError('Error: no Mesh.')

        self.BuildView()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
