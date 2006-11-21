#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import sys

import vmtkrenderer

import pypes

vmtkmeshclipper = 'vmtkMeshClipper'

class vmtkMeshClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Actor = None
        self.BoxWidget = None
        self.Planes = None

        self.SetScriptName('vmtkmeshclipper')
        self.SetScriptDoc('interactively clip a mesh with a box')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'the input mesh','vmtkmeshreader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'external renderer']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'the output mesh','vmtkmeshwriter']
            ])

    def KeyPressed(self,object,event):
        key = object.GetKeySym()
        if key != 'space':
            return
        if self.BoxWidget.GetEnabled() != 1:
            return
        self.BoxWidget.GetPlanes(self.Planes)
        self.Clipper.Update()
        self.Mesh.DeepCopy(self.Clipper.GetClippedOutput())
        self.Mesh.Update()
        self.BoxWidget.Off()

    def Display(self):

      	self.BoxWidget.SetInput(self.Mesh)
      	self.BoxWidget.PlaceWidget()

      	self.vmtkRenderer.RenderWindowInteractor.Initialize()

        self.vmtkRenderer.RenderWindowInteractor.AddObserver("KeyPressEvent", self.KeyPressed)
        self.vmtkRenderer.RenderWindow.Render()
        self.vmtkRenderer.RenderWindowInteractor.Start()

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        self.Planes = vtk.vtkPlanes()
        self.Clipper = vtk.vtkClipDataSet()
        self.Clipper.SetInput(self.Mesh)
        self.Clipper.SetClipFunction(self.Planes)
        self.Clipper.GenerateClippedOutputOn()
        self.Clipper.InsideOutOn()
        
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        mapper = vtk.vtkDataSetMapper()
        mapper.SetInput(self.Mesh)
        mapper.ScalarVisibilityOff()
        self.Actor = vtk.vtkActor()
        self.Actor.SetMapper(mapper)
        self.vmtkRenderer.Renderer.AddActor(self.Actor)

        self.BoxWidget = vtk.vtkBoxWidget()
        self.BoxWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.BoxWidget.GetFaceProperty().SetColor(0.6,0.6,0.2)
        self.BoxWidget.GetFaceProperty().SetOpacity(0.25)

        self.Display()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()
        
        if self.Mesh.GetSource():
            self.Mesh.GetSource().UnRegisterAllOutputs()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
