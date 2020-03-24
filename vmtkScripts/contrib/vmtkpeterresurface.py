#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkpeterresurface.py,v $
## Language:  Python
## Date:      JAN 05, 2021
## Version:   1.4

##   Copyright (c) Peter Patalano, Luca Antiga. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Peter Patalano (Github @ppatalano

from __future__ import absolute_import
# load Python library
import sys

# load 2 main classes of pypeS
from vmtk import pypes
from vmtk import vmtkscripts
from vmtk import vmtkrenderer

# define class peterResurface


class vmtkPeterResurface(pypes.pypeScript):
    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Display = 1

        self.vmtkRenderer = None
        self.ownRenderer = 0
        self.Script = None

        self.SetScriptName('vmtkPeterResurface')
        self.SetScriptDoc('Custom module for resurfacing vessel following computational stenosis')

        self.SetInputMembers([
            ['Surface', 'i', 'vtkPolyData', 1, '', 'the input surface', 'vmtksurfacereader'],
            ['vmtkRenderer', 'renderer', 'vmtkRenderer', 1, '', 'external renderer']
            ])
        self.SetOutputMembers([
            ['Surface', 'o', 'vtkPolyData', 1, '', 'the input surface', 'vmtksurfacewriter'],
            ['Transform', 'otransform', 'vtkTransform', 1, '', 'the transform of the clipped surface'],
            ])

    # Validate answer as y or n, returns 0 for anything else

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

    def Execute(self):
        # Error handling
        if self.Surface == None:
            self.PrintError('Error: no input surface was supplied, ensure file exists, ensure correct environment')

        # Allocate a renderer and label own renderer in use to prevent multiple renderers from being used
        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        # Render and build view of the input surface
        self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer

        self.SurfaceViewer.Surface = self.Surface
        self.SurfaceViewer.BuildView()

        # Test question in render window
        #queryStr = 'Is this the question you wanted to see? '
        #inputStr = self.YesNoInput(queryStr, self.YesNoValidator)

        # Surface smoothing, try defaults first then take user input if not adequate
        acceptableResult = 0
        response = 0
        while acceptableResult == 0:
            if response == 0:
                self.SurfaceSmoothing = vmtkscripts.vmtkSurfaceSmoothing()
                self.SurfaceSmoothing.NumberOfIterations = 100
                self.PassBand = 0.001
            elif response == 1:
                self.SurfaceViewer.Surface = self.Surface
                self.SurfaceViewer.BuildView()
                self.SurfaceSmoothing = vmtkscripts.vmtkSurfaceSmoothing()

                #Take passband from user
                queryStr = 'Please enter value for pass band (0.001 - 0.1): '
                self.SurfaceSmoothing.PassBand = float(self.InputText(queryStr, self.PassBandValidator))

                #Take number of iterations from user
                queryStr = 'Please enter number of iterations (1-999): '
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
            else:
                acceptableResult = 0

            self.SurfaceViewer.Surface = self.SurfaceClipper.Surface
            self.SurfaceViewer.BuildView()

        # Create centerlines based off the acceptable clipped surface
        centerliner = vmtkscripts.vmtkCenterlines()
        centerliner.Surface = self.Surface
        centerliner.SeedSelectorName = 'openprofiles'
        centerliner.Execute()

        self.Centerlines = centerliner.Centerlines

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

        # self.SurfaceViewer.BuildView()

        # Deallocate renderer
        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main= pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
