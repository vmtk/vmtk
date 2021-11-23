#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshclipper.py,v $
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


class vmtkMeshClipper(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.ClippedMesh = None
        self.Surface = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Actor = None
        self.BoxWidget = None
        self.Planes = None

        self.InsideOut = 0

        self.Interactive = 1
        self.ClipArrayName = None
        self.ClipValue = 0.0

        self.SetScriptName('vmtkmeshclipper')
        self.SetScriptDoc('interactively clip a mesh with a box')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['InsideOut','insideout','bool',1,'','toggle switching output and clipped surfaces'],
            ['Interactive','interactive','bool',1,'','toggle clipping with a widget or with a pre-defined scalar field'],
            ['ClipArrayName','array','str',1,'','name of the array with which to clip in case of non-interactive clipping'],
            ['ClipValue','value','float',1,'','scalar value at which to perform clipping in case of non-interactive clipping'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter'],
            ['Surface','osurface','vtkPolyData',1,'','the output surface corresponding to the cut','vmtksurfacewriter'],
            ['ClippedMesh','oclipped','vtkUnstructuredGrid',1,'','the clipped mesh','vmtkmeshwriter']
            ])

    def InteractCallback(self, obj):
        pass
        #if self.BoxWidget.GetEnabled() == 1:
        #    self.BoxWidget.SetEnabled(0)
        #else:
        #    self.BoxWidget.SetEnabled(1)

    def ClipCallback(self, obj):
        if self.BoxWidget.GetEnabled() != 1:
            return
        self.BoxWidget.GetPlanes(self.Planes)
        self.Clipper.Update()
        self.Mesh.DeepCopy(self.Clipper.GetOutput())
        self.ClippedMesh.DeepCopy(self.Clipper.GetClippedOutput())
        self.Cutter.Update()
        self.Surface.DeepCopy(self.Cutter.GetOutput())
        mapper = vtk.vtkDataSetMapper()
        mapper.SetInputData(self.Mesh)
        mapper.ScalarVisibilityOff()
        self.Actor.SetMapper(mapper)
        self.BoxWidget.Off()

    def Display(self):

        self.BoxWidget.SetInputData(self.Mesh)
        self.BoxWidget.PlaceWidget()

        self.vmtkRenderer.Render()

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        self.Clipper = vtk.vtkClipDataSet()
        self.Clipper.SetInputData(self.Mesh)
        self.Clipper.GenerateClippedOutputOn()
        self.Clipper.SetInsideOut(self.InsideOut)

        if self.Interactive:

            self.Planes = vtk.vtkPlanes()
            self.Clipper.SetClipFunction(self.Planes)

            self.Cutter = vtk.vtkCutter()
            self.Cutter.SetInputData(self.Mesh)
            self.Cutter.SetCutFunction(self.Planes)

            self.ClippedMesh = vtk.vtkUnstructuredGrid()
            self.Surface = vtk.vtkPolyData()

            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            mapper = vtk.vtkDataSetMapper()
            mapper.SetInputData(self.Mesh)
            mapper.ScalarVisibilityOff()
            self.Actor = vtk.vtkActor()
            self.Actor.SetMapper(mapper)
            self.vmtkRenderer.Renderer.AddActor(self.Actor)

            self.BoxWidget = vtk.vtkBoxWidget()
            self.BoxWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
            self.BoxWidget.GetFaceProperty().SetColor(0.6,0.6,0.2)
            self.BoxWidget.GetFaceProperty().SetOpacity(0.25)

            self.vmtkRenderer.AddKeyBinding('i','Interact.', self.InteractCallback)
            self.vmtkRenderer.AddKeyBinding('space','Clip.', self.ClipCallback)

            self.Display()

            if self.OwnRenderer:
                self.vmtkRenderer.Deallocate()

        else:

            self.Mesh.GetPointData().SetActiveScalars(self.ClipArrayName)

            self.Clipper.GenerateClipScalarsOff()
            self.Clipper.SetValue(self.ClipValue)
            self.Clipper.Update()

            self.Cutter = vtk.vtkContourFilter()
            self.Cutter.SetInputData(self.Mesh)
            self.Cutter.SetValue(0,self.ClipValue)
            self.Cutter.Update()

            self.Mesh = self.Clipper.GetOutput()
            self.Surface = self.Cutter.GetOutput()
            self.ClippedMesh = self.Clipper.GetClippedOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
