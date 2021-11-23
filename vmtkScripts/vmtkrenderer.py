#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkrenderer.py,v $
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
import os
import math

from vmtk import pypes
from vmtk import vtkvmtk


class vmtkRendererInputStream(object):

    def __init__(self,renderer):
        self.renderer = renderer

    def readline(self):
        self.renderer.EnterTextInputMode()
        return self.renderer.CurrentTextInput

    def prompt(self,text,info=False):
        self.renderer.TextInputQuery = text
        self.renderer.CurrentTextInput = None
        self.renderer.UpdateTextInput()


class vmtkRenderer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.vmtkRenderer = self

        self.Renderer = None
        self.RenderWindow = None
        self.RenderWindowInteractor = None

        self.Camera = None

        self.WindowSize = [1200, 900]
        self.WindowPosition = [50, 50]
        self.Background = [0.1, 0.1, 0.2]
        #Solarized base03
        #self.Background = [0.02734375, 0.16796875, 0.2109375]

        self.Annotations = 1

        self.PointSmoothing = 1
        self.LineSmoothing = 1
        self.PolygonSmoothing = 0

        self.TextInputMode = 0
        self.ExitAfterTextInputMode = True
        self.ExitTextInputCallback = None

        self.TextInputActor = None
        self.TextInputQuery = None

        self.CurrentTextInput = None
        self.InputPosition = [0.35, 0.1]

        self.TextActor = None
        self.Position = [0.001, 0.05]

        self.KeyBindings = {}

        self.ScreenshotMagnification = 4

        self.UseRendererInputStream = True

        self.SetScriptName('vmtkrenderer')
        self.SetScriptDoc('renderer used to make several viewers use the same rendering window')
        self.SetInputMembers([
            ['WindowSize','size','int',2,'','size of the rendering window in pixels'],
            ['WindowPosition', 'position', 'int', 2, '', 'position of the rendering window (top left pixel)'],
            ['PointSmoothing','pointsmoothing','bool',1,'','toggle rendering smooth points'],
            ['LineSmoothing','linesmoothing','bool',1,'','toggle rendering smooth lines'],
            ['PolygonSmoothing','polygonsmoothing','bool',1,'','toggle rendering smooth polygons'],
            ['Annotations','annotations','bool',1,'','toggle rendering of annotations superimposed to the renderer'],
            ['Background','background','float',3,'','background color of the rendering window'],
            ['ScreenshotMagnification','magnification','int',1,'','magnification to apply to the rendering window when taking a screenshot']])
        self.SetOutputMembers([
            ['vmtkRenderer','o','vmtkRenderer',1,'','the renderer']])

    def _GetScreenFontSize(self):
        '''Returns an integer representing font size appropriatly scaled for the users screen resolution

        This function is designed to be called from the self.Initialize() method. It creates a temporary render window
        and returns an intiger for which the text font size should be set to, given the user's current screen resolution.
        Minimum output size is set to 8, maximum to 50, for readability purposes.  

        Arguments:
            Input:
                - None (no positional or keyword arguments)

            Output:
                - fontSize (int): font size appropriatly scaled for the users screen
        '''
        tempRenderWindow = vtk.vtkRenderWindow()
        userScreenWidth, userScreenHeight = tempRenderWindow.GetScreenSize()

        baseScreenWidth = 3360 # Number of pixels across the width of the calibration monitor
        baseScreenHeight = 2100 # Number of pixels across the heigh of the calibration monitor
        baseFontSize = 24 # Font size appropriate for the calibration monitor of size baseScreenWidth x baseScreenHeight

        baseFontScalePerPixelWidth = math.floor(baseScreenWidth / baseFontSize)

        scaledFontSize = math.ceil(userScreenWidth / baseFontScalePerPixelWidth)

        # make sure that the font size won't be set too low or high for low/high screen resolutions.
        if scaledFontSize < 8:
            scaledFontSize = 8

        if scaledFontSize > 50:
            scaledFontSize = 50

        return scaledFontSize

    def ResetCameraCallback(self,obj):
        self.Renderer.ResetCamera()

    def ScreenshotCallback(self, obj):
        filePrefix = 'vmtk-screenshot'
        fileNumber = 0
        fileName = "%s-%d.png" % (filePrefix,fileNumber)
        homeDir = os.getenv("HOME")
        existingFiles = os.listdir(homeDir)
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
        writer.SetInputConnection(windowToImage.GetOutputPort())
        writer.SetFileName(os.path.join(homeDir,fileName))
        writer.Write()

    def QuitRendererCallback(self, obj):
        self.PrintLog('Quit renderer')
        self.Renderer.RemoveActor(self.TextActor)
        self.RenderWindowInteractor.ExitCallback()

    def ResetCameraCallback(self, obj):
        self.Renderer.ResetCamera()
        self.RenderWindow.Render()

    def UpdateTextInput(self):
        if self.TextInputQuery:
            if self.CurrentTextInput or self.CurrentTextInput == '':
                self.TextInputActor.SetInput(self.TextInputQuery+self.CurrentTextInput+'_')
            else:
                self.TextInputActor.SetInput(self.TextInputQuery)
            self.Renderer.AddActor(self.TextInputActor)
        else:
            self.Renderer.RemoveActor(self.TextInputActor)
        self.RenderWindow.Render()

    def CharCallback(self, obj, event):
        return

    def KeyPressCallback(self, obj, event):
        key = self.RenderWindowInteractor.GetKeySym()
        if key =='Escape':
            if self.TextInputMode:
                self.TextInputMode = 0
            else:
                self.TextInputMode = 1
        if self.TextInputMode:
            if key in ['Return','Enter']:
                self.ExitTextInputMode()
                return
            if key.startswith('KP_'):
                key = key[3:]
            if key == 'space':
                key = ' '
            elif key in ['minus','Subtract']:
                key = '-'
            elif key in ['period','Decimal']:
                key = '.'
            elif len(key) > 1 and key not in ['Backspace','BackSpace']:
                key = None
            if key in ['Backspace','BackSpace']:
                textInput = self.CurrentTextInput
                if len(textInput) > 0:
                    self.CurrentTextInput = textInput[:-1]
            elif key:
                self.CurrentTextInput += key
            self.UpdateTextInput()
            return

        if key in self.KeyBindings and self.KeyBindings[key]['callback'] != None:
            self.KeyBindings[key]['callback'](obj)
        else:
            if key == 'plus':
                key = '+'
            if key == 'minus':
                key = '-'
            if key == 'equal':
                key = '='
            if key in self.KeyBindings and self.KeyBindings[key]['callback'] != None:
                self.KeyBindings[key]['callback'](obj)

    def AddKeyBinding(self, key, text, callback=None, group='1'):
        self.KeyBindings[key] = {'text': text, 'callback': callback, 'group': group}

    def RemoveKeyBinding(self, key):
        if key in self.KeyBindings:
            del self.KeyBindings[key]

    def PromptAsync(self, queryText, callback):
        self.TextInputQuery = queryText
        self.CurrentTextInput = None
        self.ExitTextInputCallback = callback
        self.UpdateTextInput()
        self.EnterTextInputMode(interactive=0)

    def EnterTextInputMode(self,interactive=1):
        self.CurrentTextInput = ''
        self.Renderer.AddActor(self.TextInputActor)
        self.Renderer.RemoveActor(self.TextActor)
        self.UpdateTextInput()
        self.TextInputMode = 1
        self.Render(interactive)

    def ExitTextInputMode(self):
        self.Renderer.RemoveActor(self.TextInputActor)
        self.Renderer.AddActor(self.TextActor)
        self.RenderWindow.Render()
        self.TextInputMode = 0
        if self.ExitTextInputCallback:
            self.ExitTextInputCallback(self.CurrentTextInput)
            self.ExitTextInputCallback = None
        if self.ExitAfterTextInputMode:
            self.RenderWindowInteractor.ExitCallback()

    def Render(self,interactive=1):

        if interactive:
            self.RenderWindowInteractor.Initialize()
        self.RenderWindow.SetWindowName("vmtk - the Vascular Modeling Toolkit")

        #sortedKeysStd = self.KeyBindingsStd.keys()
        #sortedKeysStd.sort()
        #textActorInputsStd = ['%s: %s' % (key, self.KeyBindingsStd[key]['text']) for key in sortedKeysStd]
        #self.TextActorStd.SetInput('\n'.join(textActorInputsStd))
        #self.Renderer.AddActor(self.TextActorStd)

        groups = list(set([self.KeyBindings[el]['group'] for el in self.KeyBindings]))
        groups.sort(reverse=True)

        textActorInputsList = []

        for group in groups:
            sortedKeys = [key for key in list(self.KeyBindings.keys()) if self.KeyBindings[key]['group'] == group]
            sortedKeys.sort()
            textActorInputs = ['%s: %s' % (key, self.KeyBindings[key]['text']) for key in sortedKeys]
            textActorInputsList.append('\n'.join(textActorInputs))

        self.TextActor.SetInput('\n\n'.join(textActorInputsList))
        self.Renderer.AddActor(self.TextActor)

        #if len(self.KeyBindingsOpmode.keys()) != 0:
        #    sortedKeysOpmode = self.KeyBindingsOpmode.keys()
        #    sortedKeysOpmode.sort()
        #    textActorInputsOpmode = ['%s: %s' % (key, self.KeyBindingsOpmode[key]['text']) for key in sortedKeysOpmode]
        #    self.TextActorOpmode.SetInput('\n'.join(textActorInputsOpmode))
        #    self.TextActorOpmode.GetProperty().SetColor(1.0, 0.75, 0.32)
        #    self.Renderer.AddActor(self.TextActorOpmode)
        #else:
        #    self.TextActorOpmode.SetInput('.')
        #    self.Renderer.AddActor(self.TextActorOpmode)

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
            self.RenderWindow.SetPosition(self.WindowPosition[0],self.WindowPosition[1])
            self.RenderWindow.SetPointSmoothing(self.PointSmoothing)
            self.RenderWindow.SetLineSmoothing(self.LineSmoothing)
            self.RenderWindow.SetPolygonSmoothing(self.PolygonSmoothing)
            self.RenderWindowInteractor = vtk.vtkRenderWindowInteractor()
            #if 'vtkCocoaRenderWindowInteractor' in dir(vtk) and vtk.vtkCocoaRenderWindowInteractor.SafeDownCast(self.RenderWindowInteractor):
            #    self.RenderWindowInteractor = vtkvmtk.vtkvmtkCocoaRenderWindowInteractor()
            self.RenderWindow.SetInteractor(self.RenderWindowInteractor)
            self.RenderWindowInteractor.SetInteractorStyle(vtkvmtk.vtkvmtkInteractorStyleTrackballCamera())
            self.RenderWindowInteractor.GetInteractorStyle().KeyPressActivationOff()
            self.RenderWindowInteractor.GetInteractorStyle().AddObserver("CharEvent",self.CharCallback)
            self.RenderWindowInteractor.GetInteractorStyle().AddObserver("KeyPressEvent",self.KeyPressCallback)

            self.AddKeyBinding('x','Take screenshot.',self.ScreenshotCallback,'0')
            self.AddKeyBinding('r','Reset camera.',self.ResetCameraCallback,'0')
            #self.AddKeyBinding('w','Show wireframe.',None,'0')
            #self.AddKeyBinding('r','Reset camera.',self.ResetCameraCallback, '0')
            #self.AddKeyBinding('s','Show surface.', None,'0')
            #self.AddKeyBinding('e','Quit renderer.',self.QuitRendererCallback,'0')
            self.AddKeyBinding('q','Quit renderer/proceed.',self.QuitRendererCallback,'0')
            #self.AddKeyBinding('3','3D.', None,'0')

            #self.TextActorStd = vtk.vtkTextActor()
            #self.TextActorStd.SetPosition(self.PositionStd)
            #self.Renderer.AddActor(self.TextActorStd)

            fontSize = int(self._GetScreenFontSize())

            self.TextActor = vtk.vtkTextActor()
            self.TextActor.GetTextProperty().SetFontSize(fontSize)
            self.TextActor.GetPositionCoordinate().SetCoordinateSystemToNormalizedViewport()
            self.TextActor.GetPosition2Coordinate().SetCoordinateSystemToNormalizedViewport()
            self.TextActor.SetPosition(self.Position)
            self.Renderer.AddActor(self.TextActor)

            #self.TextActorOpmode = vtk.vtkTextActor()
            #self.TextActorOpmode.SetPosition(self.PositionOpmode)
            #self.Renderer.AddActor(self.TextActorOpmode)

            self.TextInputActor = vtk.vtkTextActor()
            self.TextInputActor.GetTextProperty().SetFontSize(fontSize)
            self.TextInputActor.GetPositionCoordinate().SetCoordinateSystemToNormalizedViewport()
            self.TextInputActor.GetPosition2Coordinate().SetCoordinateSystemToNormalizedViewport()
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
        #if 'vtkCocoaRenderWindowInteractor' in dir(vtk) and vtkvmtk.vtkvmtkCocoaRenderWindowInteractor.SafeDownCast(self.RenderWindowInteractor):
        #    self.RenderWindowInteractor.AddObserver("TimerEvent", self.Close)
        #    self.RenderWindowInteractor.CreateOneShotTimer(1)
        #    self.RenderWindowInteractor.Start()
        self.RenderWindowInteractor = None
        self.RenderWindow = None
        self.Renderer = None


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
