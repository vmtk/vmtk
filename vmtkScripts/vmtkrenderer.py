#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkrenderer.py,v $
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

import pypes

vmtkrenderer = 'vmtkRenderer'

class vmtkRenderer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.vmtkRenderer = self

        self.Renderer = None
        self.RenderWindow = None
        self.RenderWindowInteractor = None

        self.WindowSize = [800, 600]
        self.Background = [0.1, 0.1, 0.2]

        self.PointSmoothing = 1
        self.LineSmoothing = 1
        self.PolygonSmoothing = 0

        self.SetScriptName('vmtkrenderer')
        self.SetScriptDoc('renderer used to make several viewers use the same rendering window')
        self.SetInputMembers([
            ['WindowSize','size','int',2,'size of the rendering window'],
            ['PointSmoothing','pointsmoothing','int',1,'toggle rendering smooth points'],
            ['LineSmoothing','linesmoothing','int',1,'toggle rendering smooth lines'],
            ['PolygonSmoothing','polygonsmoothing','int',1,'toggle rendering smooth polygons'],
            ['Background','background','float',3,'background color of the rendering window']])
        self.SetOutputMembers([
            ['vmtkRenderer','o','vmtkRenderer',1,'the renderer']])

    def Render(self,interactive=1):

        if interactive:
            self.RenderWindowInteractor.Initialize()
        self.RenderWindow.Render()
        if interactive:
            self.RenderWindowInteractor.Start()

    def Initialize(self):

        if not self.Renderer:
            self.Renderer = vtk.vtkRenderer()
            self.Renderer.SetBackground(self.Background)
            self.RenderWindow = vtk.vtkRenderWindow()
            self.RenderWindow.AddRenderer(self.Renderer)
            self.RenderWindow.SetSize(self.WindowSize[0],self.WindowSize[1])
            self.RenderWindow.SetPointSmoothing(self.PointSmoothing)
            self.RenderWindow.SetLineSmoothing(self.LineSmoothing)
            self.RenderWindow.SetPolygonSmoothing(self.PolygonSmoothing)
            self.RenderWindowInteractor = vtk.vtkRenderWindowInteractor()
            self.RenderWindow.SetInteractor(self.RenderWindowInteractor)

    def Execute(self):
        self.Initialize()
 
    def Deallocate(self):
        self.RenderWindowInteractor = None
        self.RenderWindow = None
        self.Renderer = None
   
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
