#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagepainter.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Tangui Morvan
##       Kalkulo AS
##       Simula Research Laboratory

## This class allows the user to paint a 3D image

import vtk
import sys

import vtkvmtk
import vmtkrenderer
import pypes

vmtkimagepainter = 'vmtkImagePainter'

class vmtkImagePainter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.PaintedImage = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.Picker = None
        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None
        
        self.MaxUndo = 10000
        self.Strokes = []
        self.StrokesLength = 0
        
        self.BrushRadius = 0
        self.BrushValue = 1000.
        self.BrushMask = True
        self.Brush = [0.]
        self.BrushDims = [0,0,0]
        
        self.TextureInterpolation = 1

        self.Epsilon = 1E-5

        self.SetScriptName('vmtkimagepainter')
        self.SetScriptDoc('interactively place seeds in a 3D image')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes'],
            ['MaxUndo','maxundo','int',1,'','maximum number of undos']
            ])
        self.SetOutputMembers([
            ['PaintedImage','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])
            
    def PaintPoint(self, coords, value, saveToStroke):
        wholeExtent = self.PaintedImage.GetWholeExtent()
        for i in range(3):
            if (coords[i]<wholeExtent[2*i]) or (coords[i]>wholeExtent[2*i+1]):
                return
        if saveToStroke and (len(self.Strokes)>0):
            self.Strokes[-1].append([coords,self.PaintedImage.GetScalarComponentAsFloat(int(coords[0]),int(coords[1]),int(coords[2]),0)])
            self.StrokesLength = self.StrokesLength + 1
            if self.StrokesLength > self.MaxUndo:
                self.StrokesLength = self.StrokesLength - len(self.Strokes[0])
                del self.Strokes[0]
            
        self.PaintedImage.SetScalarComponentFromFloat(int(coords[0]),int(coords[1]),int(coords[2]),0,value)
        
    
    def PaintBrush(self, coords):
        wholeExtent = self.PaintedImage.GetWholeExtent()
        brushExtent = [0, 0, 0, 0, 0, 0]
        #Check that the brush intersects the extent
        for i in range(3):
            brushExtent[2*i] = int(max(coords[i] - (self.BrushDims[i]-1)/2,wholeExtent[2*i]))
            brushExtent[2*i+1] = int(min(coords[i] + (self.BrushDims[i]-1)/2,wholeExtent[2*i+1]))
            if (brushExtent[2*i] > wholeExtent[2*i+1]) or (brushExtent[2*i+1] < wholeExtent[2*i]):
                return
        for i in range(brushExtent[0],brushExtent[1]+1):
            iInd = i - coords[0] + (self.BrushDims[0]-1)/2
            for j in range(brushExtent[2],brushExtent[3]+1):
                jInd = j - coords[1] + (self.BrushDims[1]-1)/2
                for k in range(brushExtent[4],brushExtent[5]+1):
                    kInd = k - coords[2] + (self.BrushDims[2]-1)/2
                    ind = int(iInd*self.BrushDims[1]* self.BrushDims[2] + jInd*self.BrushDims[2] + kInd)
                    
                    if self.BrushMask[ind]:
                        self.PaintPoint([i, j, k], self.Brush[ind], True)
        self.PaintedImage.Modified()
        self.vmtkRenderer.RenderWindow.Render()
        
    def EraseBrush(self, coords):
        wholeExtent = self.PaintedImage.GetWholeExtent()
        brushExtent = [0, 0, 0, 0, 0, 0]
        #Check that the brush intersects the extent
        for i in range(3):
            brushExtent[2*i] = int(max(coords[i] - (self.BrushDims[i]-1)/2,wholeExtent[2*i]))
            brushExtent[2*i+1] = int(min(coords[i] + (self.BrushDims[i]-1)/2,wholeExtent[2*i+1]))
            if (brushExtent[2*i] > wholeExtent[2*i+1]) or (brushExtent[2*i+1] < wholeExtent[2*i]):
                return
        for i in range(brushExtent[0],brushExtent[1]+1):
            iInd = i - coords[0] + (self.BrushDims[0]-1)/2
            for j in range(brushExtent[2],brushExtent[3]+1):
                jInd = j - coords[1] + (self.BrushDims[1]-1)/2
                for k in range(brushExtent[4],brushExtent[5]+1):
                    kInd = k - coords[2] + (self.BrushDims[2]-1)/2
                    ind = int(iInd*self.BrushDims[1]* self.BrushDims[2] + jInd*self.BrushDims[2] + kInd)
                    
                    if self.BrushMask[ind]:
                        self.PaintPoint([i, j, k], self.Image.GetScalarComponentAsFloat(i,j,k,0), True)
        self.PaintedImage.Modified()
        self.vmtkRenderer.RenderWindow.Render()
                        
    def SphereBrush(self):
        self.BrushDims = [2*self.BrushRadius+1, 2*self.BrushRadius+1, 2*self.BrushRadius+1]
        self.Brush = []
        self.BrushMask = []
        for i in range(self.BrushDims[0]):
            for j in range(self.BrushDims[1]):
                for k in range(self.BrushDims[2]):
                    dist2 = (i - self.BrushRadius)*(i - self.BrushRadius ) + (j - self.BrushRadius)*(j - self.BrushRadius) + (k - self.BrushRadius)*(k - self.BrushRadius)
                    if (dist2 <= self.BrushRadius*self.BrushRadius + self.Epsilon):
                        self.BrushMask.append(True)
                        self.Brush.append(self.BrushValue)
                    else:
                        self.BrushMask.append(False)
                        self.Brush.append(0)
                        
                        
    def BrushValueValidator(self,text):
        if not text:
            return 1
        try:
            float(text)
        except ValueError:
            return 0
        return 1
                        
    def ResetCallback(self,obj):
        self.PaintedImage.DeepCopy(self.Image)
        self.vmtkRenderer.RenderWindow.Render()
        
    def UndoCallback(self, obj):
        if (len(self.Strokes) == 0):
            return
        stroke = self.Strokes.pop()
        stroke.reverse()
        self.StrokesLength = self.StrokesLength - len(stroke)
        for point in stroke:
            self.PaintPoint(point[0],point[1],False)
        self.PaintedImage.Modified()
        self.vmtkRenderer.RenderWindow.Render()
        
                    
    def IncreaseRadiusCallback(self,obj):
        self.BrushRadius = self.BrushRadius + 1
        self.SphereBrush()
        
    def DecreaseRadiusCallback(self,obj):
        self.BrushRadius = self.BrushRadius - 1
        if (self.BrushRadius < 0):
            self.BrushRadius = 0
        self.SphereBrush()
        
    def BrushValueCallback(self,obj):
          queryString = 'Please input new brush value('+str(self.BrushValue)+'):'
          inputString = self.InputText(queryString,self.BrushValueValidator)
          self.BrushValue = float(inputString)
          self.SphereBrush()
          self.vmtkRenderer.Render()
        
    def PickValueCallback(self,obj):
        cursorData = [0, 0, 0, 0]
        if self.PlaneWidgetX.GetCursorDataStatus() != 0:
            self.PlaneWidgetX.GetCursorData(cursorData)
        elif self.PlaneWidgetY.GetCursorDataStatus() != 0:
            self.PlaneWidgetY.GetCursorData(cursorData)
        elif self.PlaneWidgetZ.GetCursorDataStatus() != 0:
            self.PlaneWidgetZ.GetCursorData(cursorData)
        else:
            return
        self.BrushValue = cursorData[3]
        self.SphereBrush()
        
                
    
    def StartInteraction(self, obj, event):
        cursorData = [0.0,0.0,0.0,0.0]
        obj.GetCursorData(cursorData)
        paint = (self.vmtkRenderer.RenderWindowInteractor.GetControlKey() != 0)
        erase = (self.vmtkRenderer.RenderWindowInteractor.GetShiftKey() != 0)
        if (self.vmtkRenderer.RenderWindowInteractor.GetAltKey() != 0):
            self.BrushValue = cursorData[3]
            self.SphereBrush()
        if (self.DrawingStroke) and (not paint) and (not erase):
            return
        self.DrawingStroke = True
        if (len(self.Strokes) == 0) or (len(self.Strokes[-1]) != 0):
            self.Strokes.append([])
        if paint:
            self.PaintBrush(cursorData[0:3])
        elif erase:
            self.EraseBrush(cursorData[0:3])
        
    def Interaction(self, obj, event):
        paint = (self.vmtkRenderer.RenderWindowInteractor.GetControlKey() != 0)
        erase = (self.vmtkRenderer.RenderWindowInteractor.GetShiftKey() != 0)
        if (not paint) and (not erase):
            if self.DrawingStroke:
                self.DrawingStroke = False
            return
        else:
            if (not self.DrawingStroke) and ((len(self.Strokes) == 0) or (len(self.Strokes[-1]) != 0)):
                self.Strokes.append([])
            self.DrawingStroke = True
            cursorData = [0.0,0.0,0.0,0.0]
            obj.GetCursorData(cursorData)
            if paint:
                self.PaintBrush(cursorData[0:3])
            elif erase:
                self.EraseBrush(cursorData[0:3])
        
    def EndInteraction(self, obj, event):
        if  (self.DrawingStroke):
            self.DrawingStroke = False
        
    def WidgetsOn(self):
        self.PlaneWidgetX.On()
        self.PlaneWidgetY.On()
        self.PlaneWidgetZ.On()

    def WidgetsOff(self):
        self.PlaneWidgetX.Off()
        self.PlaneWidgetY.Off()
        self.PlaneWidgetZ.Off()

    def BuildView(self):

        if (self.ArrayName != ''):
            self.PaintedImage.GetPointData().SetActiveScalars(self.ArrayName)
        wholeExtent = self.PaintedImage.GetWholeExtent()
#        self.PlaneWidgetX.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetX.SetResliceInterpolateToLinear()
        self.PlaneWidgetX.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetX.SetInput(self.PaintedImage)
        self.PlaneWidgetX.SetPlaneOrientationToXAxes()
        self.PlaneWidgetX.SetSliceIndex(wholeExtent[0])
        self.PlaneWidgetX.DisplayTextOn()
        self.PlaneWidgetX.KeyPressActivationOn()
        
#        self.PlaneWidgetY.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetY.SetResliceInterpolateToLinear()
        self.PlaneWidgetY.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetY.SetInput(self.PaintedImage)
        self.PlaneWidgetY.SetPlaneOrientationToYAxes()
        self.PlaneWidgetY.SetSliceIndex(wholeExtent[2])
        self.PlaneWidgetY.DisplayTextOn()
        self.PlaneWidgetY.KeyPressActivationOn()
        self.PlaneWidgetY.SetLookupTable(self.PlaneWidgetX.GetLookupTable())

#        self.PlaneWidgetZ.SetResliceInterpolateToNearestNeighbour()
        self.PlaneWidgetZ.SetResliceInterpolateToLinear()
        self.PlaneWidgetZ.SetTextureInterpolate(self.TextureInterpolation)
        self.PlaneWidgetZ.SetInput(self.PaintedImage)
        self.PlaneWidgetZ.SetPlaneOrientationToZAxes()
        self.PlaneWidgetZ.SetSliceIndex(wholeExtent[4])
        self.PlaneWidgetZ.DisplayTextOn()
        self.PlaneWidgetZ.KeyPressActivationOn()
        self.PlaneWidgetZ.SetLookupTable(self.PlaneWidgetX.GetLookupTable())

        self.WidgetsOn()
        
        self.Strokes = []
        self.StrokesLength = 0
        self.DrawingStroke = False
        
        self.BrushRadius = 0
        
        dataRange = [0.,0.]
        self.Image.GetScalarRange(dataRange)
        self.BrushValue = dataRange[1]
                        
        self.SphereBrush()

        if (self.Display == 1):
            self.vmtkRenderer.AddKeyBinding('Ctrl','Draw.')
            self.vmtkRenderer.AddKeyBinding('Shift','Erase.')
            self.vmtkRenderer.AddKeyBinding('space','Pick brush value.',self.PickValueCallback)
            self.vmtkRenderer.AddKeyBinding('v','Set brush value.',self.BrushValueCallback)
            self.vmtkRenderer.AddKeyBinding('p','Increase brush radius.',self.IncreaseRadiusCallback)
            self.vmtkRenderer.AddKeyBinding('o','Decrease brush radius.',self.DecreaseRadiusCallback)
            self.vmtkRenderer.AddKeyBinding('r','Reset to original.',self.ResetCallback)
            self.vmtkRenderer.AddKeyBinding('u','Undo latest stroke.',self.UndoCallback)
            self.vmtkRenderer.Render()


    def Execute(self):
        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1
            
        self.PaintedImage = vtk.vtkImageData()
        self.PaintedImage.DeepCopy(self.Image)

        self.vmtkRenderer.RegisterScript(self) 

        ##self.PrintLog('Ctrl +  left click to add seed.')
        self.Picker = vtk.vtkCellPicker()
        self.Picker.SetTolerance(0.005)

        self.PlaneWidgetX = vtk.vtkImagePlaneWidget()
        self.PlaneWidgetX.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetX.AddObserver("StartInteractionEvent", self.StartInteraction)
        self.PlaneWidgetX.AddObserver("InteractionEvent", self.Interaction)
        self.PlaneWidgetX.AddObserver("EndInteractionEvent", self.EndInteraction)
        self.PlaneWidgetX.SetPicker(self.Picker)
        self.PlaneWidgetY = vtk.vtkImagePlaneWidget()
        self.PlaneWidgetY.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetY.AddObserver("StartInteractionEvent", self.StartInteraction)
        self.PlaneWidgetY.AddObserver("InteractionEvent", self.Interaction)
        self.PlaneWidgetY.AddObserver("EndInteractionEvent", self.EndInteraction)
        self.PlaneWidgetY.SetPicker(self.Picker)
        self.PlaneWidgetZ = vtk.vtkImagePlaneWidget()
        self.PlaneWidgetZ.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        self.PlaneWidgetZ.AddObserver("StartInteractionEvent", self.StartInteraction)
        self.PlaneWidgetZ.AddObserver("InteractionEvent", self.Interaction)
        self.PlaneWidgetZ.AddObserver("EndInteractionEvent", self.EndInteraction)
        self.PlaneWidgetZ.SetPicker(self.Picker)

        self.BuildView()
        
        self.WidgetsOff()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
