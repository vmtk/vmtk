#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkpetergeneratesurface.py,v $
## Language:  Python
## Date:      JAN 05, 2021
## Version:   1.4

##   Copyright (c) Peter Patalano, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Peter Patalano (Github @ppatalano)


from __future__ import absolute_import
# load Python library
import sys

# load 2 main classes of pypeS
from vmtk import pypes
from vmtk import vmtkscripts
from vmtk import vmtkrenderer


# Class definition
# Will need to contain an image as input and renderer
# Output will be a surface and a transform

class vmtkPeterGenerateSurface(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.Image = None
        self.Surface = None
        self.Transform = None
        self.Display = 1

        self.vmtkRenderer = None
        self.ownRenderer = 0
        self.Script = None

        self.SetScriptName('vmtkPeterGenerateSurface')
        self.SetScriptDoc('Custom module for generating a surface from image data')

        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','the input image','vmtkimagereader'],
            ['vmtkRenderer', 'renderer', 'vmtkRenderer', 1, '', 'external renderer']
            ])
        self.SetOutputMembers([
            ['Surface', 'o', 'vtkPolyData', 1, '', 'the input surface', 'vmtksurfacewriter'],
            ['Transform', 'otransform', 'vtkTransform', 1, '', 'the transform of the clipped surface'],
            ])

    def YesNoValidator(self, text):
        if text in ['n','y']:
            return 1
        return 0

    # Validator for number of iterations; inputs of 1 to 999 are valid
    def IterationValidator(self, text):
        if int(text) in list(range(1,1000)):
            return 1
        return 0

    # Validatory for passband; passband must fall between 0.1 and 0.001
    def PassBandValidator(self, text):
        upper = 0.1
        lower = 0.001
        try:
            text = float(text)
        except ValueError:
            return 0
        text = float(text)
        if text <= upper and text >= lower:
            return 1
        return 0

    # Validatory for extension ratio; recommend extension ratio from 10-20, allow for 1-50
    # To be altered in the case of pipe collision
    def ExtensionRatioValidator(self, text):
        if int(text) in list(range(1,51)):
            return 1
        return 0

    # Define method for taking yes or no input which will only be valid for y or n input
    def YesNoInput(self, queryStr, YesNoValidator):

        YesOrNo = self.InputText(queryStr, YesNoValidator)
        if YesOrNo == 'y':
            return 1
        if YesOrNo == 'n':
            return 0
        else:
            self.PrintError('Error: invalid input')

    # Main method
    def Execute(self):
        # Error handling
        if self.Image == None:
            self.PrintError('Error: no image was supplied')

        # Allocate a renderer and label own renderer in use to prevent multiple renderers from being used
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        # VOI selector
        acceptableResult = 0
        while acceptableResult == 0:
            self.ImageVOISelector = vmtkscripts.vmtkImageVOISelector()
            self.ImageVOISelector.Image = self.Image
            self.ImageVOISelector.Execute()

            queryStr = 'Accept image volume? (y/n): '
            if(self.YesNoInput(queryStr, self.YesNoValidator)):
                acceptableResult = 1
                self.Image = self.ImageVOISelector.Image
            else:
                acceptableResult = 0

        # Level set segmenation and marching cubes
        acceptableResult = 0
        while acceptableResult == 0:

            self.Surface = None

            self.LevelSetSegmentation = vmtkscripts.vmtkLevelSetSegmentation()
            self.LevelSetSegmentation.Image = self.Image
            self.LevelSetSegmentation.Execute()
            self.Image = self.LevelSetSegmentation.Image

            self.MarchingCubes = vmtkscripts.vmtkMarchingCubes()
            self.MarchingCubes.Image = self.Image
            self.MarchingCubes.Execute()
            self.Surface = self.MarchingCubes.Surface

            self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
            self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer

            self.SurfaceViewer.Surface = self.Surface
            self.SurfaceViewer.BuildView()

            queryStr = 'Accept results of level set and marching cubes? (y/n): '
            if(self.YesNoInput(queryStr, self.YesNoValidator)):
                acceptableResult = 1
            else:
                acceptableResult = 0

        # Surface smoothing, try defaults first then take user input if not adequate
        acceptableResult = 0
        response = 0
        while acceptableResult == 0:
            if response == 0:
                self.SurfaceSmoothing = vmtkscripts.vmtkSurfaceSmoothing()
                self.SurfaceSmoothing.NumberOfIterations = 50
                self.PassBand = 0.01
            elif response == 1:
                self.SurfaceViewer.Surface = self.Surface
                self.SurfaceViewer.BuildView()
                self.SurfaceSmoothing = vmtkscripts.vmtkSurfaceSmoothing()

                #Take passband from user
                queryStr = 'Please enter value for pass band [default = 0.01] (0.001 - 0.1): '
                self.SurfaceSmoothing.PassBand = float(self.InputText(queryStr, self.PassBandValidator))

                #Take number of iterations from user
                queryStr = 'Please enter number of iterations [default = 50] (1-999): '
                self.SurfaceSmoothing.NumberOfIterations = int(self.InputText(queryStr, self.IterationValidator))

            self.SurfaceSmoothing.Surface = self.Surface
            self.SurfaceSmoothing.Execute()
            self.SurfaceViewer.Surface = self.SurfaceSmoothing.Surface
            self.SurfaceViewer.BuildView()

            # Accept or reject result of smoothing surface
            queryStr = 'Accept result of smoothing?(y/n): '
            if(self.YesNoInput(queryStr, self.YesNoValidator)):
                acceptableResult = 1
                self.Surface = self.SurfaceSmoothing.Surface
            else:
                acceptableResult = 0
                response = 1

        # Surface clipping
        # The only issue here is that 2 renderers get built 2/2 the structure of vmtkSurfaceClipper
        # Issue to be revisited later
        acceptableResult = 0
        while acceptableResult == 0:

            self.SurfaceClipper = vmtkscripts.vmtkSurfaceClipper()
            self.SurfaceClipper.Surface = self.Surface
            self.SurfaceClipper.Execute()

            self.SurfaceViewer.Surface = self.SurfaceClipper.Surface
            self.SurfaceViewer.BuildView()

            queryStr = 'Accept result of clipping?(y/n): '
            if(self.YesNoInput(queryStr, self.YesNoValidator)):
                acceptableResult = 1
                self.Surface = self.SurfaceClipper.Surface
                self.Transform = self.SurfaceClipper.Transform
            else:
                acceptableResult = 0

            self.SurfaceViewer.Surface = self.SurfaceClipper.Surface
            self.SurfaceViewer.BuildView()

        # Surface subdivision
        # Performs subdivision using butterfly method w/ default settings

        self.SurfaceSubdivision = vmtkscripts.vmtkSurfaceSubdivision()
        self.SurfaceSubdivision.Surface = self.Surface
        self.SurfaceSubdivision.Method = 'butterfly'

        self.SurfaceSubdivision.Exectute()

        self.Surface = self.SurfaceSubdivision.Surface

        # Add flow extensions until user has acceptable result
        acceptableResult = 0
        response = 0
        while(acceptableResult == 0):

            self.FlowExtensions = vmtkscripts.vmtkFlowExtensions()
            self.FlowExtensions.Surface = self.Surface
            self.FlowExtensions.Centerlines = self.Centerlines
            self.FlowExtensions.AdaptiveExtensionLength = 1
            #self.FlowExtensions.CenterlineNormalEstimationDistanceRatio = 1
            self.FlowExtensions.Interactive = 0

            # Take user extension ratio if response to acceptable outcome question
            # Default extension ratio is 10
            if(response == 0):
                self.FlowExtensions.ExtensionRatio = 10
            elif(response == 1):
                #Take extension ratio from user
                queryStr = 'Please enter value for pass band; default is 20 (min/max 1-50): '
                self.FlowExtensions.ExtensionRatio = float(self.InputText(queryStr, self.ExtensionRatioValidator))

            self.FlowExtensions.Execute()

            self.SurfaceViewer.Surface = self.FlowExtensions.Surface
            self.SurfaceViewer.BuildView()

            queryStr = 'Accept flow extensions?(y/n): '
            if(self.YesNoInput(queryStr, self.YesNoValidator)):
                acceptableResult = 1
                self.Surface = self.FlowExtensions.Surface
            else:
                acceptableResult = 0
                response = 1
