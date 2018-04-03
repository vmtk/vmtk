#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevolumeviewer.py,v $
## Language:  Python
## Date:      $Date: 2018/04/02 12:35:13 $
## Version:   $Revision: 0.1 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import vmtkimageviewer
from vmtk import pypes

class vmtkImageVolumeViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.EnableImagePlaneWidgets = 0
        self.Picker = None
        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None

        self.EnableGPURendering = 1
        self.Margins = 0
        self.TextureInterpolation = 1
        self.ContinuousCursor = 0
        self.WindowLevel = [0.0, 0.0]

        self.SetScriptName('vmtkimagevolumeviewer')
        self.SetScriptDoc('display a 3D image as a volume, optionally overlaying image planes')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['WindowLevel','windowlevel','float',2,'','the window/level for displaying the image'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Margins','margins','bool',1,'','toggle margins for tilting image planes'],
            ['EnableGPURendering','enablegpu','bool',1,'','toggle gpu vs cpu rendering method, if enabled and no gpu is installed will default to cpu rendering'],
            ['EnableImagePlaneWidgets','enableimageplanes','bool',0,'','show the volume with image planes overlayed on the image'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes'],
            ['ContinuousCursor','continuouscursor','bool',1,'','toggle use of physical continuous coordinates for the cursor']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['PlaneWidgetX','xplane','vtkImagePlaneWidget',1,'','the X image plane widget'],
            ['PlaneWidgetY','yplane','vtkImagePlaneWidget',1,'','the Y image plane widget'],
            ['PlaneWidgetZ','zplane','vtkImagePlaneWidget',1,'','the Z image plane widget']
            ])

    def CharCallback(self, obj):
        return

    def BuildImagePlanes(self):
        '''Call vmtkImageViewer with the same render window to show the image plane widget overlays.

        Requires that self.vmtkRenderer is instantiated
        '''

        imageViewer = vmtkimageviewer.vmtkImageViewer()
        imageViewer.Image = self.Image
        imageViewer.Render = self.vmtkRenderer
        imageViewer.Display = 1
        imageViewer.Execute()

        return


    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)
        
        if self.EnableImagePlaneWidgets == True:
            #TODO: Figure this out
            pass

        tfun = vtk.vtkPiecewiseFunction()
        tfun.AddPoint(70.0, 0.0)
        tfun.AddPoint(59.0, 0)
        tfun.AddPoint(60.0, 0)
        tfun.AddPoint(119.0, 0)
        tfun.AddPoint(120, .2)
        tfun.AddPoint(130, .3)
        tfun.AddPoint(200, .3)
        tfun.AddPoint(255.0, 1.0)

        ctfun = vtk.vtkColorTransferFunction()
        ctfun.AddRGBPoint(0.0, 0.5, 0.0, 0.0)
        ctfun.AddRGBPoint(60.0, 1.0, 0.5, 0.5)
        ctfun.AddRGBPoint(120.0, 0.9, 0.2, 0.3)
        ctfun.AddRGBPoint(200.0, 0.81, 0.27, 0.1)
        ctfun.AddRGBPoint(255.0, 0.5, 0.5, 0.5)

        volumeMapper = vtk.vtkGPUVolumeRayCastMapper()
        volumeMapper.SetInputData(self.Image)
        volumeMapper.SetBlendModeToComposite()

        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetColor(ctfun)
        volumeProperty.SetScalarOpacity(tfun)
        volumeProperty.SetInterpolationTypeToLinear()
        volumeProperty.ShadeOn()

        newvol = vtk.vtkVolume()
        newvol.SetMapper(volumeMapper)
        newvol.SetProperty(volumeProperty)

        outline = vtk.vtkOutlineFilter()
        outline.SetInputData(self.Image)
        outlineMapper = vtk.vtkPolyDataMapper()
        outlineMapper.SetInputConnection(outline.GetOutputPort())
        outlineActor = vtk.vtkActor()
        outlineActor.SetMapper(outlineMapper)

        # Create the RenderWindow, Renderer and both Actors
        # ren = vtk.vtkRenderer()
        # renWin = vtk.vtkRenderWindow()
        # renWin.AddRenderer(ren)
        # iren = vtk.vtkRenderWindowInteractor()
        # iren.SetRenderWindow(renWin)

        # The SetInteractor method is how 3D widgets are associated with the
        # render window interactor. Internally, SetInteractor sets up a bunch
        # of callbacks using the Command/Observer mechanism (AddObserver()).
        boxWidget = vtk.vtkBoxWidget()
        boxWidget.SetInteractor(iren)
        boxWidget.SetPlaceFactor(1.0)

        # Add the actors to the renderer, set the background and size
        self.vmtkRenderer.AddActor(outlineActor)
        self.vmtkRenderer.AddVolume(newvol)

        # When interaction starts, the requested frame rate is increased.
        # def StartInteraction(obj, event):
        #     global renWin
        #     renWin.SetDesiredUpdateRate(10)

        # When interaction ends, the requested frame rate is decreased to
        # normal levels. This causes a full resolution render to occur.
        # def EndInteraction(obj, event):
        #     global renWin
        #     renWin.SetDesiredUpdateRate(0.001)

        # The implicit function vtkPlanes is used in conjunction with the
        # volume ray cast mapper to limit which portion of the volume is
        # volume rendered.
        planes = vtk.vtkPlanes()
        def ClipVolumeRender(obj, event):
            global planes, volumeMapper
            obj.GetPlanes(planes)
            volumeMapper.SetClippingPlanes(planes)


        # Place the interactor initially. The output of the reader is used to
        # place the box widget.
        boxWidget.SetInputData(self.Image)
        boxWidget.PlaceWidget()
        boxWidget.InsideOutOn()
        boxWidget.AddObserver("StartInteractionEvent", StartInteraction)
        boxWidget.AddObserver("InteractionEvent", ClipVolumeRender)
        boxWidget.AddObserver("EndInteractionEvent", EndInteraction)

        outlineProperty = boxWidget.GetOutlineProperty()
        outlineProperty.SetRepresentationToWireframe()
        outlineProperty.SetAmbient(1.0)
        outlineProperty.SetAmbientColor(1, 1, 1)
        outlineProperty.SetLineWidth(3)

        selectedOutlineProperty = boxWidget.GetSelectedOutlineProperty()
        selectedOutlineProperty.SetRepresentationToWireframe()
        selectedOutlineProperty.SetAmbient(1.0)
        selectedOutlineProperty.SetAmbientColor(1, 0, 0)
        selectedOutlineProperty.SetLineWidth(3)

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')
 
        self.BuildView()