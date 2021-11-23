#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkflowextensions.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:52:56 $
## Version:   $Revision: 1.7 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import pypes
from vmtk import vmtkrenderer


class vmtkFlowExtensions(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None

        self.AdaptiveExtensionLength = 0
        self.AdaptiveExtensionRadius = 1
        self.AdaptiveNumberOfBoundaryPoints = 0
        self.ExtensionLength = 1.0
        self.ExtensionRatio = 10.0
        self.ExtensionRadius = 1.0
        self.TransitionRatio = 0.25
        self.TargetNumberOfBoundaryPoints = 50
        self.CenterlineNormalEstimationDistanceRatio = 1.0
        self.ExtensionMode = "centerlinedirection"
        self.InterpolationMode = "thinplatespline"
        self.Interactive = 1
        self.Exclude = 0
        self.Sigma = 1.0

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.SetScriptName('vmtkflowextensions')
        self.SetScriptDoc('interactivly add flow extensions to a surface for CFD processing')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['ExtensionMode','extensionmode','str',1,'["centerlinedirection","boundarynormal"]','method for computing the normal for extension'],
            ['InterpolationMode','interpolationmode','str',1,'["linear","thinplatespline"]','method for computing interpolation from the model section to a circular section'],
            ['Sigma','sigma','float',1,'(0.0,)','thin plate spline stiffness'],
            ['AdaptiveExtensionLength','adaptivelength','bool',1],
            ['AdaptiveExtensionRadius','adaptiveradius','bool',1],
            ['AdaptiveNumberOfBoundaryPoints','adaptivepoints','bool',1],
            ['ExtensionLength','extensionlength','float',1,'(0.0,)'],
            ['ExtensionRatio','extensionratio','float',1,'(0.0,)'],
            ['ExtensionRadius','extensionradius','float',1,'(0.0,)'],
            ['TransitionRatio','transitionratio','float',1,'(0.0,)'],
            ['TargetNumberOfBoundaryPoints','boundarypoints','int',1,'(0,)'],
            ['Interactive','interactive','bool',1],
            ['Exclude','exclude','bool',1,'', 'create extensions for all boundaries except those selected'],
            ['vmtkRenderer','renderer','vmtkRenderer',1,'','external renderer'],
            ['CenterlineNormalEstimationDistanceRatio','normalestimationratio','float',1,'(0.0,)']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['Centerlines','centerlines','vtkPolyData',1]
            ])

    def LabelValidator(self,text):
        import string
        if not text:
            return 0
        if not text.split():
            return 0
        for char in text:
            if char not in string.digits + " ":
                return 0
        return 1

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.ExtensionMode == "centerlinedirection" and self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        boundaryIds = vtk.vtkIdList()

        if self.Interactive:
            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkrenderer.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            boundaryExtractor = vtkvmtk.vtkvmtkPolyDataBoundaryExtractor()
            boundaryExtractor.SetInputData(self.Surface)
            boundaryExtractor.Update()
            boundaries = boundaryExtractor.GetOutput()
            numberOfBoundaries = boundaries.GetNumberOfCells()
            seedPoints = vtk.vtkPoints()
            for i in range(numberOfBoundaries):
                barycenter = [0.0, 0.0, 0.0]
                vtkvmtk.vtkvmtkBoundaryReferenceSystems.ComputeBoundaryBarycenter(boundaries.GetCell(i).GetPoints(),barycenter)
                seedPoints.InsertNextPoint(barycenter)
            seedPolyData = vtk.vtkPolyData()
            seedPolyData.SetPoints(seedPoints)
            labelsMapper = vtk.vtkLabeledDataMapper();
            labelsMapper.SetInputData(seedPolyData)
            labelsMapper.SetLabelModeToLabelIds()
            labelsActor = vtk.vtkActor2D()
            labelsActor.SetMapper(labelsMapper)

            self.vmtkRenderer.Renderer.AddActor(labelsActor)

            surfaceMapper = vtk.vtkPolyDataMapper()
            surfaceMapper.SetInputData(self.Surface)
            surfaceMapper.ScalarVisibilityOff()
            surfaceActor = vtk.vtkActor()
            surfaceActor.SetMapper(surfaceMapper)
            surfaceActor.GetProperty().SetOpacity(0.25)

            self.vmtkRenderer.Renderer.AddActor(surfaceActor)

            #self.vmtkRenderer.Render()

            #self.vmtkRenderer.Renderer.RemoveActor(labelsActor)
            #self.vmtkRenderer.Renderer.RemoveActor(surfaceActor)

            ok = False
            while not ok:
                if(self.Exclude):
                    labelString = self.InputText("Please input boundary ids to exclude: ",self.LabelValidator)
                else:
                    labelString = self.InputText("Please input boundary ids: ",self.LabelValidator)
                labels = [int(label) for label in labelString.split()]
                ok = True
                for label in labels:
                    if label not in list(range(numberOfBoundaries)):
                        ok = False

            if(self.Exclude):
                for label in list(range(numberOfBoundaries)):
                    if label not in labels:
                        boundaryIds.InsertNextId(label)
            else:
                for label in labels:
                    boundaryIds.InsertNextId(label)

        flowExtensionsFilter = vtkvmtk.vtkvmtkPolyDataFlowExtensionsFilter()
        flowExtensionsFilter.SetInputData(self.Surface)
        flowExtensionsFilter.SetCenterlines(self.Centerlines)
        flowExtensionsFilter.SetSigma(self.Sigma)
        flowExtensionsFilter.SetAdaptiveExtensionLength(self.AdaptiveExtensionLength)
        flowExtensionsFilter.SetAdaptiveExtensionRadius(self.AdaptiveExtensionRadius)
        flowExtensionsFilter.SetAdaptiveNumberOfBoundaryPoints(self.AdaptiveNumberOfBoundaryPoints)
        flowExtensionsFilter.SetExtensionLength(self.ExtensionLength)
        flowExtensionsFilter.SetExtensionRatio(self.ExtensionRatio)
        flowExtensionsFilter.SetExtensionRadius(self.ExtensionRadius)
        flowExtensionsFilter.SetTransitionRatio(self.TransitionRatio)
        flowExtensionsFilter.SetCenterlineNormalEstimationDistanceRatio(self.CenterlineNormalEstimationDistanceRatio)
        flowExtensionsFilter.SetNumberOfBoundaryPoints(self.TargetNumberOfBoundaryPoints)
        if self.ExtensionMode == "centerlinedirection":
            flowExtensionsFilter.SetExtensionModeToUseCenterlineDirection()
        elif self.ExtensionMode == "boundarynormal":
            flowExtensionsFilter.SetExtensionModeToUseNormalToBoundary()
        if self.InterpolationMode == "linear":
            flowExtensionsFilter.SetInterpolationModeToLinear()
        elif self.InterpolationMode == "thinplatespline":
            flowExtensionsFilter.SetInterpolationModeToThinPlateSpline()
        if self.Interactive:
            flowExtensionsFilter.SetBoundaryIds(boundaryIds)
        flowExtensionsFilter.Update()

        self.Surface = flowExtensionsFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
