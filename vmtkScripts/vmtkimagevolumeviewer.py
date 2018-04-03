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
        self.Volume = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.OpacityTransferFunctionPoints = None
        self.ColorTransferFunctionRGBPoints = None
        self.GradientOpacityTransferFunctionPoints = None

        self.EnableImagePlaneWidgets = 0
        self.Picker = None
        self.PlaneWidgetX = None
        self.PlaneWidgetY = None
        self.PlaneWidgetZ = None

        self.VolumeRenderingMethod = 'default'
        self.Margins = 0
        self.TextureInterpolation = 1
        self.ContinuousCursor = 1
        self.WindowLevel = [0.0, 0.0]

        self.SetScriptName('vmtkimagevolumeviewer')
        self.SetScriptDoc('display a 3D image as a volume, optionally overlaying image planes')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['OpacityTransferFunctionPoints','opacitypoints','list','','[(imageScalarMinValue, 0.0), (imageScalarMaxValue * 0.9, 1.0)]','sets image opacity. requires a list of tupples each of size 2 corresponding to image scalar value and desired opacity value at that point'],
            ['ColorTransferFunctionRGBPoints','colorrgbpoints','list','','[(imageScalarMinValue, 0.5, 0.0, 0.0), (imageScalarMaxValue, 0.7, 0.5, 0.5)]','sets image opacity. requires a list of tupples each of size 2 corresponding to image scalar value and desired opacity value at that point'],
            ['GradientOpacityTransferFunctionPoints','gradientopacitypoints','list','','[(imageScalarMinValue, 0.0), (imageScalarMaxValue, 1.0)]','sets image gradient opacity. requires a list of tupples each of size 2 corresponding to image scalar value and desired gradient opacity value at that point'],
            ['VolumeRenderingMethod','rendermethod','str',1,'["default",gpu","ospray","raycast"]','toggle rendering method. By default will auto detect hardware capabilities and select best render method. If desired, can set to RayCast, OSPRay, or GPU rendering. Note, may crash if GPU method is set and no compatible GPU is available on the system. Suggested to leave at default value.'],
            ['EnableImagePlaneWidgets','enableimageplanes','bool',1,'','show the volume with image planes overlayed on the image'],
            ['Margins','margins','bool',1,'','toggle margins for tilting image planes. (only if EnableImagePlaneWidgets == True)'],
            ['WindowLevel','windowlevel','float',2,'','the window/level for displaying the image. (only if EnableImagePlaneWidgets == True)'],
            ['TextureInterpolation','textureinterpolation','bool',1,'','toggle interpolation of graylevels on image planes. (only if EnableImagePlaneWidgets == True)'],
            ['ContinuousCursor','continuouscursor','bool',1,'','toggle use of physical continuous coordinates for the cursor. (only if EnableImagePlaneWidgets == True)']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['Volume','o','vtkVolume',1,'','the output volume with'],
            ['PlaneWidgetX','xplane','vtkImagePlaneWidget',1,'','the X image plane widget (only if EnableImagePlaneWidgets == True)'],
            ['PlaneWidgetY','yplane','vtkImagePlaneWidget',1,'','the Y image plane widget (only if EnableImagePlaneWidgets == True)'],
            ['PlaneWidgetZ','zplane','vtkImagePlaneWidget',1,'','the Z image plane widget (only if EnableImagePlaneWidgets == True)']
            ])

    def CharCallback(self, obj):
        return

    def BuildImagePlanes(self):
        '''Call vmtkImageViewer with the same render window to show the image plane widget overlays.

        Requires that self.vmtkRenderer is instantiated
        '''

        imageViewer = vmtkimageviewer.vmtkImageViewer()
        imageViewer.Image = self.Image
        imageViewer.vmtkRenderer = self.vmtkRenderer
        imageViewer.Display = 0
        imageViewer.WindowLevel = self.WindowLevel
        imageViewer.ArrayName = self.ArrayName
        imageViewer.TextureInterpolation = self.TextureInterpolation
        imageViewer.ContinuousCursor = self.ContinuousCursor
        imageViewer.Margins = self.Margins
        imageViewer.Execute()

        self.PlaneWidgetX = imageViewer.PlaneWidgetX
        self.PlaneWidgetY = imageViewer.PlaneWidgetY
        self.PlaneWidgetZ = imageViewer.PlaneWidgetZ

    def BuildVTKPiecewiseFunction(self, pointsList):
        '''Assign values to a newly created vtkPiecewiseFunction

        Arguments:
            Inputs:
                pointsList (list of tuples): a list of tuples, each of size=2. The first value
                    specifies the xValue of the point to add (aka image scalar value). The second
                    value specifies the yValue of the point to add (aka, value at that image scalar)

            Outputs:
                piecewiseFunction (vtkPiecewiseFunction object): containing points at tupples in the points list
        '''

        piecewiseFunction = vtk.vtkPiecewiseFunction()

        for xValue, yValue in pointsList:
            # xValue, yValue = xyTuple[0], xyTuple[1]
            piecewiseFunction.AddPoint(xValue, yValue)

        return piecewiseFunction


    def BuildVTKColorTransferFunction(self, pointsList):
        '''Assign values to a newly created vtkColorTransferFunction

        Arguments:
            Inputs:
                pointsList (list of tuples): a list of tuples, each of size=4. The first value
                    specifies the xValue of the point to add (aka image scalar value). The second,
                    third, and fourth values specify the RGB value to assign at that point

            Outputs:
                colorTransferFunction (vtkColorTransferFunction object): containing RGB points at tupples in the points list
        '''

        colorTransferFunction = vtk.vtkColorTransferFunction()

        for xValue, RVal, GVal, BVal in pointsList:
            # xValue, yValue = xyTuple[0], xyTuple[1]
            colorTransferFunction.AddRGBPoint(xValue, RVal, GVal, BVal)

        return colorTransferFunction
        

    def BuildView(self):

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self) 

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)
        
        if self.EnableImagePlaneWidgets == True:
            self.BuildImagePlanes()

        # ensure python 2-3 compatibility for checking string type (difference with unicode str handeling)
        PY3 = sys.version_info[0] == 3
        if PY3:
            string_types = str,
        else:
            string_types = basestring,
        if not isinstance(self.VolumeRenderingMethod, string_types):
            self.PrintError('Specified Rendering Method is not of required "string" type')
            
        # create volume mapper and apply requested volume rendering method
        volumeMapper = vtk.vtkSmartVolumeMapper()
        if self.VolumeRenderingMethod.lower() == 'default':
            volumeMapper.SetRequestedRenderModeToDefault()
        elif self.VolumeRenderingMethod.lower() == 'gpu':
            volumeMapper.SetRequestedRenderModeToGPU()
        elif self.VolumeRenderingMethod.lower() == 'ospray':
            volumeMapper.SetRequestedRenderModeToOSPRay()
        elif self.VolumeRenderingMethod.lower() == 'raycast':
            volumeMapper.SetRequestedRenderModeToRayCast()
        else:
            self.PrintError('Specified Rendering Method: ' + self.VolumeRenderingMethod + ' not supported. Please choose from ["default", "gpu", "ospray", "raycast"]')
        volumeMapper.SetInputData(self.Image)
        volumeMapper.SetBlendModeToComposite()


        imageScalarRange = self.Image.GetScalarRange() # tupple of (min, max) scalar values in image volume
        # need to ensure float type as expected by vtkPiecewiseFunction 
        imageScalarMinValue = float(imageScalarRange[0])
        imageScalarMaxValue = float(imageScalarRange[1])
        
        # build transfer functions
        if self.OpacityTransferFunctionPoints is not None:
            opacityTransferFunction = self.BuildVTKPiecewiseFunction(self.ColorTransferFunctionRGBPoints)
        else:
            opacityTransferFunction = self.BuildVTKPiecewiseFunction([(imageScalarMinValue, 0.0), 
                                                                      (imageScalarMaxValue * 0.9, 1.0)])

        if self.GradientOpacityTransferFunctionPoints is not None:
            gradientOpacityTransferFunction = self.BuildVTKPiecewiseFunction(self.ColorTransferFunctionRGBPoints)
        else:
            gradientOpacityTransferFunction = self.BuildVTKPiecewiseFunction([(imageScalarMinValue, 0.0), 
                                                                              (imageScalarMaxValue, 1.0)])

        if self.ColorTransferFunctionRGBPoints is not None:
            colorTransferFunction = self.BuildVTKColorTransferFunction(self.ColorTransferFunctionRGBPoints)
        else:
            colorTransferFunction = self.BuildVTKColorTransferFunction([(imageScalarMinValue, 0.5, 0.0, 0.0),
                                                                        (imageScalarMaxValue, 0.9, 0.5, 0.3)])
        
        # set transfer function properties 
        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetScalarOpacity(opacityTransferFunction)
        volumeProperty.SetGradientOpacity(gradientOpacityTransferFunction)
        volumeProperty.SetColor(colorTransferFunction)
        volumeProperty.SetInterpolationTypeToLinear()
        volumeProperty.ShadeOn()

        # apply transfer function properties to the volume
        self.Volume = vtk.vtkVolume()
        self.Volume.SetMapper(volumeMapper)
        self.Volume.SetProperty(volumeProperty)

        outline = vtk.vtkOutlineFilter()
        outline.SetInputData(self.Image)
        outlineMapper = vtk.vtkPolyDataMapper()
        outlineMapper.SetInputConnection(outline.GetOutputPort())
        outlineActor = vtk.vtkActor()
        outlineActor.SetMapper(outlineMapper)

        # The SetInteractor method is how 3D widgets are associated with the
        # render window interactor. Internally, SetInteractor sets up a bunch
        # of callbacks using the Command/Observer mechanism (AddObserver()).
        boxWidget = vtk.vtkBoxWidget()
        boxWidget.SetInteractor(self.vmtkRenderer.RenderWindowInteractor)
        boxWidget.SetPlaceFactor(1.0)

        # Place the interactor initially. The output of the reader is used to
        # place the box widget.
        boxWidget.SetInputData(self.Image)
        boxWidget.PlaceWidget()
        boxWidget.InsideOutOn()

        # Add the actors to the renderer, set the background and size
        self.vmtkRenderer.Renderer.AddActor(outlineActor)
        self.vmtkRenderer.Renderer.AddVolume(self.Volume)

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')
 
        self.BuildView()