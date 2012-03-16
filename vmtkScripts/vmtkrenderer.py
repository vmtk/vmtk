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
import os

import pypes
import vtkvmtk

vmtkrenderer = 'vmtkRenderer'


class vmtkRendererInputStream(object):

    def __init__(self,renderer):
        self.renderer = renderer

    def readline(self):
        self.renderer.EnterTextInputMode(exitAfter=True)
        return self.renderer.CurrentTextInput

    def prompt(self,text):
        self.renderer.TextInputQuery = text


class vmtkRenderer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.vmtkRenderer = self

        self.Renderer = None
        self.RenderWindow = None
        self.RenderWindowInteractor = None

        self.Camera = None

        self.WindowSize = [800, 600]
        self.Background = [0.1, 0.1, 0.2]

        self.PointSmoothing = 1
        self.LineSmoothing = 1
        self.PolygonSmoothing = 0

        self.TextInputMode = 0

        self.TextInputActor = None
        self.TextInputQuery = None

        self.CurrentTextInput = None
        self.InputPosition = [150.0, 150.0]

        self.TextActor = None
        self.Position = [5.0, 150.0]
        self.TextActorStd = None
        self.PositionStd = [5.0, 40.0]
        self.TextActorOpmode = None
        self.PositionOpmode = [5.0, 250.0]

        self.KeyBindings = {}
        self.KeyBindingsStd = {}
        self.KeyBindingsOpmode = {}
 
        self.ScreenshotMagnification = 4

        self.UseRendererInputStream = True

        self.SetScriptName('vmtkrenderer')
        self.SetScriptDoc('renderer used to make several viewers use the same rendering window')
        self.SetInputMembers([
            ['WindowSize','size','int',2,'','size of the rendering window'],
            ['PointSmoothing','pointsmoothing','bool',1,'','toggle rendering smooth points'],
            ['LineSmoothing','linesmoothing','bool',1,'','toggle rendering smooth lines'],
            ['PolygonSmoothing','polygonsmoothing','bool',1,'','toggle rendering smooth polygons'],
            ['Background','background','float',3,'','background color of the rendering window'],
            ['ScreenshotMagnification','magnification','int',1,'','magnification to apply to the rendering window when taking a screenshot']])
        self.SetOutputMembers([
            ['vmtkRenderer','o','vmtkRenderer',1,'','the renderer']])

    def ScreenshotCallback(self, obj):
        filePrefix = 'vmtk-screenshot'
        fileNumber = 0
        fileName = "%s-%d.png" % (filePrefix,fileNumber)
        existingFiles = os.listdir('.')
        while fileName in existingFiles:
            fileNumber += 1
            fileName = "%s-%d.png" % (filePrefix,fileNumber)
        self.PrintLog('Saving screenshot to ' + fileName)
        windowToImage = vtk.vtkWindowToImageFilter()
        windowToImage.SetInput(self.RenderWindow)
        windowToImage.SetMagnification(self.ScreenshotMagnification)
        windowToImage.Update()
        self.RenderWindow.Render()
        writer = vtk.vtkPNGWriter()
        writer.SetInput(windowToImage.GetOutput())
        writer.SetFileName(fileName)
        writer.Write()

    def QuitRendererCallback(self, obj):
        print 'Exiting'
        self.Renderer.RemoveActor(self.TextActor)
        self.RenderWindowInteractor.ExitCallback()

    def UpdateTextInput(self):
        self.TextInputActor.SetInput(self.TextInputQuery+self.CurrentTextInput+'_')
        self.RenderWindow.Render()

    def CharCallback(self, obj, event):
        key = self.RenderWindowInteractor.GetKeySym()

        if self.TextInputMode:
            if key == 'Return':
                self.ExitTextInputMode()
                return
            if key == 'space':
                key = ' '
            elif len(key) > 1 and key != 'Backspace':
                key = None
            if key == 'Backspace':
                textInput = self.CurrentTextInput
                if len(textInput) > 0:
                    self.CurrentTextInput = textInput[:-1]
            elif key:
                self.CurrentTextInput += key
            self.UpdateTextInput()
            return

        if key in self.KeyBindings and self.KeyBindings[key]['callback'] != None:
            self.KeyBindings[key]['callback'](obj)
        if key in self.KeyBindingsStd and self.KeyBindingsStd[key]['callback'] != None:
            self.KeyBindingsStd[key]['callback'](obj)
        if key in self.KeyBindingsOpmode and self.KeyBindingsOpmode[key]['callback'] != None:
            self.KeyBindingsOpmode[key]['callback'](obj)

    def AddKeyBinding(self, key, text, callback=None, group=None):
        if group == 'std':
            self.KeyBindingsStd[key] = {'text': text, 'callback': callback}
        elif group == 'opmode': 
            self.KeyBindingsOpmode[key] = {'text': text, 'callback': callback}
        else:
            self.KeyBindings[key] = {'text': text, 'callback': callback}

    def RemoveKeyBinding(self, key):
        if key in self.KeyBindings:    
            del self.KeyBindings[key]
        elif key in self.KeyBindingsOpmode:
            del self.KeyBindingsOpmode[key]

    def EnterTextInputMode(self, exitAfter=False):
        self.CurrentTextInput = ''
        self.Renderer.AddActor(self.TextInputActor)
        self.UpdateTextInput()
        self.TextInputMode = 1
        self.ExitAfterTextInput = exitAfter
        self.Render()
    
    def ExitTextInputMode(self):
        self.Renderer.RemoveActor(self.TextInputActor)
        self.RenderWindow.Render()
        self.TextInputMode = 0
        if self.ExitAfterTextInput:
            self.RenderWindowInteractor.ExitCallback()
            self.ExitAfterTextInput = False

    def Render(self,interactive=1):

        if interactive:
            self.RenderWindowInteractor.Initialize()
        self.RenderWindow.SetWindowName("vmtk - the Vascular Modeling Toolkit")

        sortedKeysStd = self.KeyBindingsStd.keys()
        sortedKeysStd.sort()
        textActorInputsStd = ['%s: %s' % (key, self.KeyBindingsStd[key]['text']) for key in sortedKeysStd]
        self.TextActorStd.SetInput('\n'.join(textActorInputsStd))
        self.Renderer.AddActor(self.TextActorStd)
    
        if len(self.KeyBindings.keys()) != 0:
            sortedKeys = self.KeyBindings.keys()
            sortedKeys.sort()
            textActorInputs = ['%s: %s' % (key, self.KeyBindings[key]['text']) for key in sortedKeys]
            self.TextActor.SetInput('\n'.join(textActorInputs))
            self.Renderer.AddActor(self.TextActor)

        if len(self.KeyBindingsOpmode.keys()) != 0:
            sortedKeysOpmode = self.KeyBindingsOpmode.keys()
            sortedKeysOpmode.sort()
            textActorInputsOpmode = ['%s: %s' % (key, self.KeyBindingsOpmode[key]['text']) for key in sortedKeysOpmode]
            self.TextActorOpmode.SetInput('\n'.join(textActorInputsOpmode))
            self.TextActorOpmode.GetProperty().SetColor(1.0, 0.75, 0.32)
            self.Renderer.AddActor(self.TextActorOpmode)
        else:
            self.TextActorOpmode.SetInput('.')
            self.Renderer.AddActor(self.TextActorOpmode)
 
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
            if 'vtkCocoaRenderWindowInteractor' in dir(vtk) and vtk.vtkCocoaRenderWindowInteractor.SafeDownCast(self.RenderWindowInteractor):
                self.RenderWindowInteractor = vtkvmtk.vtkvmtkCocoaRenderWindowInteractor()
            self.RenderWindow.SetInteractor(self.RenderWindowInteractor)
            self.RenderWindowInteractor.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())
            self.RenderWindowInteractor.GetInteractorStyle().AddObserver("CharEvent",self.CharCallback)

            self.AddKeyBinding('x','Take screenshot.',self.ScreenshotCallback,'std')
            #self.AddKeyBinding('w','Show wideframe.',None,'std')
            #self.AddKeyBinding('r','Resize.',None, 'std')
            #self.AddKeyBinding('s','Show surface.', None,'std')
            #self.AddKeyBinding('e','Quit renderer.',self.QuitRendererCallback,'std')
            self.AddKeyBinding('q','Quit renderer and proceed.',self.QuitRendererCallback,'std')
            #self.AddKeyBinding('3','3D.', None,'std')

            self.TextActorStd = vtk.vtkTextActor()
            self.TextActorStd.SetPosition(self.PositionStd)
            self.Renderer.AddActor(self.TextActorStd)

            self.TextActor = vtk.vtkTextActor()
            self.TextActor.SetPosition(self.Position)
            self.Renderer.AddActor(self.TextActor)

            self.TextActorOpmode = vtk.vtkTextActor()
            self.TextActorOpmode.SetPosition(self.PositionOpmode)
            self.Renderer.AddActor(self.TextActorOpmode)

            self.TextInputActor = vtk.vtkTextActor()
            self.TextInputActor.SetPosition(self.InputPosition)
        
        if self.UseRendererInputStream:
            self.InputStream = vmtkRendererInputStream(self)

    def RegisterScript(self, script):
        if self.UseRendererInputStream:
            script.InputStream = vmtkRendererInputStream(self)

    def Execute(self):
        self.Initialize()

    def Close(self,event,clientData):
        self.RenderWindowInteractor.Close()

    def Deallocate(self):
        if 'vtkCocoaRenderWindowInteractor' in dir(vtk) and vtkvmtk.vtkvmtkCocoaRenderWindowInteractor.SafeDownCast(self.RenderWindowInteractor):
            self.RenderWindowInteractor.AddObserver("TimerEvent", self.Close)
            self.RenderWindowInteractor.CreateOneShotTimer(1)
            self.RenderWindowInteractor.Start()
        self.RenderWindowInteractor = None
        self.RenderWindow = None
        self.Renderer = None
   
if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
