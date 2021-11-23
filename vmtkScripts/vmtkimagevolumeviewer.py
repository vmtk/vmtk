#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimagevolumeviewer.py,v $
## Language:  Python
## Date:      $Date: 2018/04/02 12:35:13 $
## Version:   $Revision: 0.1 $

##   Copyright (c) Richard Izzo, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

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

        self.Preset = 'CT-Coronary-Arteries-2'
        self.VolumeProperty = None
        self.VolumeRenderingMethod = 'default'
        self.BoxOutline = 0

        self.ColorTransferFunction = None
        self.GradientOpacityTransferFunction = None
        self.OpacityTransferFunction = None
        self.InterpolationType = None
        self.Shade = None
        self.SpecularPower = None
        self.Specular = None
        self.Diffuse = None
        self.Ambient = None

        self.SetScriptName('vmtkimagevolumeviewer')
        self.SetScriptDoc('display a 3D image within a volume renderer')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['ArrayName','array','str',1,'','name of the array to display'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['Display','display','bool',1,'','toggle rendering'],
            ['Preset','preset','str',1,'["CT-AAA","CT-AAA2","CT-Bone","CT-Bones",\
                                        "CT-Cardiac","CT-Cardiac2","CT-Cardiac3","CT-Chest-Contrast-Enhanced",\
                                        "CT-Chest-Vessels","CT-Coronary-Arteries","CT-Coronary-Arteries-2",\
                                        "CT-Coronary-Arteries-3","CT-Cropped-Volume-Bone","CT-Fat","CT-Liver-Vasculature",\
                                        "CT-Lung","CT-MIP","CT-Muscle","CT-Pulmonary-Arteries","CT-Soft-Tissue","MR-Angio",\
                                        "MR-Default","MR-MIP","MR-T2-Brain","DTI-FA-Brain"]','sets the color map, opacity, and lighting applied to visualize the rendered volume'],
            ['VolumeRenderingMethod','rendermethod','str',1,'["default","gpu","ospray","raycast"]','toggle rendering method. By default will auto detect hardware capabilities and select best render method. If desired, can set to RayCast, OSPRay, or GPU rendering. Note, may crash if GPU method is set and no compatible GPU is available on the system. Suggested to leave at default value.'],
            ['BoxOutline','box','bool',1,'','display a box-shaped outline around the edges of the volume data']
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','the output image','vmtkimagewriter'],
            ['ColorTransferFunction','colorfunction','vtkColorTransferFunction',1,'',''],
            ['GradientOpacityTransferFunction','gradientopacityfunction','vtkPiecewiseFunction',1,'',''],
            ['OpacityTransferFunction','opacityfunction','vtkPiecewiseFunction',1,'',''],
            ['InterpolationType','interpolation','int',1,'',''],
            ['Shade','shade','int',1,'',''],
            ['SpecularPower','specularpower','float',1,'',''],
            ['Specular','specular','float',1,'',''],
            ['Diffuse','diffuse','float',1,'',''],
            ['Ambient','ambient','float',1,'','']
            ])

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

    def SetPresetVolumeProperty(self):
        '''Create the transfer functions and set volume properties in a vtkVolumeProperty object'''

        # parse the xml file which is loaded in the same path as the vmtkimagevolumeviewer.py file at runtime
        presetElementTree = ET.parse(os.path.join(os.path.dirname(__file__), 'share', 'vmtkimagevolumeviewerpresets.xml'))
        presetRoot = presetElementTree.getroot()
        volProperties = presetRoot.findall('VolumeProperty[@name="' + self.Preset + '"]') # should return a list with only one element
        volPropertiesDict = volProperties[0].attrib

        def _chunks(l, n):
            """Yield successive n-sized chunks from l."""
            PY3 = sys.version_info[0] == 3
            if PY3:
                range_func = range
            else:
                range_func = xrange
            for i in range_func(0, len(l), n):
                yield l[i:i + n]

        # need to convert the space seperated string displaying values into a list of floats
        colorList = [float(i) for i in volPropertiesDict['colorTransfer'].split()]
        gradientOpacityList = [float(i) for i in volPropertiesDict['gradientOpacity'].split()]
        opacityList = [float(i) for i in volPropertiesDict['scalarOpacity'].split()]

        # create an array of arrays with each list split into subarrays of desired size
        colorMapList = _chunks(colorList, 4)
        gradientOpacityMapList = _chunks(gradientOpacityList, 2)
        opacityMapList = _chunks(opacityList, 2)

        # create vtk objects from the mapped lists (now arrays of arrays)
        self.ColorTransferFunction = self.BuildVTKColorTransferFunction(colorMapList)
        self.GradientOpacityTransferFunction = self.BuildVTKPiecewiseFunction(gradientOpacityMapList)
        self.OpacityTransferFunction = self.BuildVTKPiecewiseFunction(opacityMapList)

        # assign other properties from the element tree to variables with the appropriate type
        self.InterpolationType = int(volPropertiesDict['interpolation'])
        self.Shade = int(volPropertiesDict['shade'])
        self.SpecularPower = float(volPropertiesDict['specularPower'])
        self.Specular = float(volPropertiesDict['specular'])
        self.Diffuse = float(volPropertiesDict['diffuse'])
        self.Ambient = float(volPropertiesDict['ambient'])

        # set transfer function and lighting properties of the vtk volume object
        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetScalarOpacity(self.OpacityTransferFunction)
        volumeProperty.SetGradientOpacity(self.GradientOpacityTransferFunction)
        volumeProperty.SetColor(self.ColorTransferFunction)
        volumeProperty.SetInterpolationType(self.InterpolationType)
        volumeProperty.SetShade(self.Shade)
        volumeProperty.SetSpecularPower(self.SpecularPower)
        volumeProperty.SetSpecular(self.Specular)
        volumeProperty.SetDiffuse(self.Diffuse)
        volumeProperty.SetAmbient(self.Ambient)

        self.VolumeProperty = volumeProperty

    def PresetCallback(self, obj):
        '''Set render window to render the next volume property when callback key is pressed'''

        if not self.VolumeProperty:
            return

        if self.Preset == "CT-AAA":
            self.Preset = "CT-AAA2"
        elif self.Preset == "CT-AAA2":
            self.Preset = "CT-Bone"
        elif self.Preset == "CT-Bone":
            self.Preset = "CT-Bones"
        elif self.Preset == "CT-Bones":
            self.Preset = "CT-Cardiac"
        elif self.Preset == "CT-Cardiac":
            self.Preset = "CT-Cardiac2"
        elif self.Preset == "CT-Cardiac2":
            self.Preset = "CT-Cardiac3"
        elif self.Preset == "CT-Cardiac3":
            self.Preset = "CT-Chest-Contrast-Enhanced"
        elif self.Preset == "CT-Chest-Contrast-Enhanced":
            self.Preset = "CT-Chest-Vessels"
        elif self.Preset == "CT-Chest-Vessels":
            self.Preset = "CT-Coronary-Arteries"
        elif self.Preset == "CT-Coronary-Arteries":
            self.Preset = "CT-Coronary-Arteries-2"
        elif self.Preset == "CT-Coronary-Arteries-2":
            self.Preset = "CT-Coronary-Arteries-3"
        elif self.Preset == "CT-Coronary-Arteries-3":
            self.Preset = "CT-Cropped-Volume-Bone"
        elif self.Preset == "CT-Cropped-Volume-Bone":
            self.Preset = "CT-Fat"
        elif self.Preset == "CT-Fat":
            self.Preset = "CT-Liver-Vasculature"
        elif self.Preset == "CT-Liver-Vasculature":
            self.Preset = "CT-Lung"
        elif self.Preset == "CT-Lung":
            self.Preset = "CT-MIP"
        elif self.Preset == "CT-MIP":
            self.Preset = "CT-Muscle"
        elif self.Preset == "CT-Muscle":
            self.Preset = "CT-Pulmonary-Arteries"
        elif self.Preset == "CT-Pulmonary-Arteries":
            self.Preset = "CT-Soft-Tissue"
        elif self.Preset == "CT-Soft-Tissue":
            self.Preset = "MR-Angio"
        elif self.Preset == "MR-Angio":
            self.Preset = "MR-Default"
        elif self.Preset == "MR-Default":
            self.Preset = "MR-MIP"
        elif self.Preset == "MR-MIP":
            self.Preset = "MR-T2-Brain"
        elif self.Preset == "MR-T2-Brain":
            self.Preset = "CT-AAA"

        self.PrintLog("switched Preset to: " + self.Preset, 1)

        self.SetPresetVolumeProperty()
        self.Volume.SetProperty(self.VolumeProperty)
        self.vmtkRenderer.RenderWindow.Render()

    def BuildView(self):

        # ensure python 2-3 compatibility for checking string type (difference with unicode str handeling)
        PY3 = sys.version_info[0] == 3

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        if (self.ArrayName != ''):
            self.Image.GetPointData().SetActiveScalars(self.ArrayName)

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

        # create the volume from the mapper (defining image data and render mode) with the defined properties
        # this is the last pipeline step applied. self.Volume just needs to now be added to the Renderer
        self.SetPresetVolumeProperty()
        self.Volume = vtk.vtkVolume()
        self.Volume.SetMapper(volumeMapper)
        self.Volume.SetProperty(self.VolumeProperty)

        # add the actors to the renderer
        self.vmtkRenderer.Renderer.AddVolume(self.Volume)

        # This next section is responsible for creating the box outline around the volume's data extent.
        if self.BoxOutline:
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

            # Add the actors to the renderer
            self.vmtkRenderer.Renderer.AddActor(outlineActor)

        # add preset callback
        self.vmtkRenderer.AddKeyBinding('w','Change volume render property.',self.PresetCallback)

        if (self.Display == 1):
            self.vmtkRenderer.Render()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()

    def Execute(self):

        if (self.Image == None) & (self.Display == 1):
            self.PrintError('Error: no Image.')

        self.BuildView()
