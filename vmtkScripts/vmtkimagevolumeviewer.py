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
import xml.etree.ElementTree as ET
import os

class vmtkImageVolumeViewer(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Volume = None
        self.vmtkRenderer = None
        self.OwnRenderer = 0
        self.Display = 1
        self.ArrayName = ''

        self.VolumeVisualizationMethod = 'CT-Coronary-Arteries-2'
        self.VolumeRenderingMethod = 'default'

        self.SetScriptName('vmtkimagevolumeviewer')
        self.SetScriptDoc('display a 3D image as a volume, optionally overlaying image planes')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['VolumeVisualizationMethod','visualizationmethod','str',1,'["CT-AAA","CT-AAA2","CT-Bone","CT-Bones",\
                                                                         "CT-Cardiac","CT-Cardiac2","CT-Cardiac3","CT-Chest-Contrast-Enhanced",\
                                                                         "CT-Chest-Vessels","CT-Coronary-Arteries","CT-Coronary-Arteries-2",\
                                                                         "CT-Coronary-Arteries-3","CT-Cropped-Volume-Bone","CT-Fat","CT-Liver-Vasculature",\
                                                                         "CT-Lung","CT-MIP","CT-Muscle","CT-Pulmonary-Arteries","CT-Soft-Tissue","MR-Angio",\
                                                                         "MR-Default","MR-MIP","MR-T2-Brain","DTI-FA-Brain"]','sets the color map, opacity, and lighting applied to visualize the rendered volume'],
            ['VolumeRenderingMethod','rendermethod','str',1,'["default",gpu","ospray","raycast"]','toggle rendering method. By default will auto detect hardware capabilities and select best render method. If desired, can set to RayCast, OSPRay, or GPU rendering. Note, may crash if GPU method is set and no compatible GPU is available on the system. Suggested to leave at default value.']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter']
            ])

    def CharCallback(self, obj):
        return

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
        
        # parse the xml file which is loaded in the same path as the vmtkimagevolumeviewer.py file at runtime
        presetElementTree = ET.parse(os.path.join(os.path.dirname(__file__), 'vmtkimagevolumeviewerpresets.xml'))
        presetRoot = presetElementTree.getroot()
        volProperties = presetRoot.findall('VolumeProperty[@name="' + self.VolumeVisualizationMethod + '"]')
        volPropertiesDict = volProperties[0].attrib

        # need to convert the space seperated string displaying values into a list of floats
        colorList = [float(i) for i in volPropertiesDict['colorTransfer'].split()]
        gradientOpacityList = [float(i) for i in volPropertiesDict['gradientOpacity'].split()]
        opacityList = [float(i) for i in volPropertiesDict['scalarOpacity'].split()]
        # remove the first element from these lists since they just indicate the number of elements in the list
        colorList.pop(0)
        gradientOpacityList.pop(0)
        opacityList.pop(0)

        def chunks(l, n):
            """Yield successive n-sized chunks from l."""
            if PY3:
                range_func = range
            else:
                range_func = xrange
            for i in range_func(0, len(l), n):
                yield l[i:i + n]

        colorMapList = chunks(colorList, 4)
        colorTransferFunction = self.BuildVTKColorTransferFunction(colorMapList)
        
        gradientOpacityMapList = chunks(gradientOpacityList, 2)
        gradientOpacityTransferFunction = self.BuildVTKPiecewiseFunction(gradientOpacityMapList)

        opacityMapList = chunks(opacityList, 2)
        opacityTransferFunction = self.BuildVTKPiecewiseFunction(opacityMapList)

        # set transfer function properties 
        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetScalarOpacity(opacityTransferFunction)
        volumeProperty.SetGradientOpacity(gradientOpacityTransferFunction)
        volumeProperty.SetColor(colorTransferFunction)
        volumeProperty.SetInterpolationType(int(volPropertiesDict['interpolation']))
        volumeProperty.SetShade(int(volPropertiesDict['shade']))
        volumeProperty.SetSpecularPower(float(volPropertiesDict['specularPower']))
        volumeProperty.SetSpecular(float(volPropertiesDict['specular']))
        volumeProperty.SetDiffuse(float(volPropertiesDict['diffuse']))
        volumeProperty.SetAmbient(float(volPropertiesDict['ambient']))

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