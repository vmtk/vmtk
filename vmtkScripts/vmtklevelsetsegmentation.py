#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklevelsetsegmentation.py,v $
## Language:  Python
## Date:      $Date: 2006/05/31 10:51:21 $
## Version:   $Revision: 1.19 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import math
import string
import vtk

from vmtk import vtkvmtk
from vmtk import pypes


class vmtkLevelSetSegmentation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.InitialLevelSets = None
        self.InitializationImage = None
        self.FeatureImage = None
        self.LevelSetsInput = None
        self.LevelSetsOutput = None
        self.LevelSets = None

        self.UpperThreshold = 0.0
        self.LowerThreshold = 0.0

        self.NumberOfIterations = 0
        self.PropagationScaling = 0.0
        self.CurvatureScaling = 0.0
        self.AdvectionScaling = 1.0

        self.IsoSurfaceValue = 0.0
        self.MaximumRMSError = 1E-20
        self.DerivativeSigma = 0.0
        self.FeatureDerivativeSigma = 0.0

        self.NegateForInitialization = 0

        self.SigmoidRemapping = 0

        self.LevelSetsType = 'geodesic'
        self.FeatureImageType = 'gradient'

        self.UpwindFactor = 1.0

        self.FWHMRadius = [1.0, 1.0, 1.0]
        self.FWHMBackgroundValue = 0.0

        self.EdgeWeight = 0.0
        self.SmoothingIterations = 5
        self.SmoothingTimeStep = 0.1
        self.SmoothingConductance = 0.8

        self.SetScriptName('vmtklevelsetsegmentation')
        self.SetScriptDoc('interactivly initialize an initial level set and evolve it to image gradients')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','','vmtkimagereader'],
            ['FeatureImage','featureimage','vtkImageData',1,'','','vmtkimagereader'],
            ['InitializationImage','initializationimage','vtkImageData',1,'','','vmtkimagereader'],
            ['InitialLevelSets','initiallevelsets','vtkImageData',1,'','','vmtkimagereader'],
            ['LevelSets','levelsets','vtkImageData',1,'','','vmtkimagereader'],
            ['LevelSetsType','levelsetstype','str',1,'["geodesic","curves","threshold","laplacian"]'],
            ['FeatureImageType','featureimagetype','str',1,'["vtkgradient","gradient","upwind","fwhm"]'],
            ['NegateForInitialization','negate','bool',1],
            ['SigmoidRemapping','sigmoid','bool',1],
            ['IsoSurfaceValue','isosurfacevalue','float',1],
            ['DerivativeSigma','derivativesigma','float',1,'(0.0,)'],
            ['FeatureDerivativeSigma','featurederivativesigma','float',1,'(0.0,)'],
            ['UpwindFactor','upwindfactor','float',1,'(0.0,1.0)'],
            ['FWHMRadius','fwhmradius','float',3,'(0.0,)'],
            ['FWHMBackgroundValue','fwhmbackgroundvalue','float',1],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['PropagationScaling','propagation','float',1,'(0.0,)'],
            ['CurvatureScaling','curvature','float',1,'(0.0,)'],
            ['AdvectionScaling','advection','float',1,'(0.0,)'],
            ['EdgeWeight','edgeweight','float',1,'(0.0,)'],
            ['SmoothingIterations','smoothingiterations','int',1,'(0,)'],
            ['SmoothingTimeStep','smoothingtimestep','float',1,'(0,)'],
            ['SmoothingConductance','smoothingconductance','float',1,'(0,)'],
            ['vmtkRenderer','renderer','vmtkRenderer',1]
            ])
        self.SetOutputMembers([
            ['LevelSets','o','vtkImageData',1,'','','vmtkimagewriter'],
            ['FeatureImage','ofeatureimage','vtkImageData',1,'','','vmtkimagewriter']
            ])

    def ThresholdValidator(self,text):
        if text == 'n':
            return 1
        if text == 'i':
            self.vmtkRenderer.Render()
            return 0
        try:
            float(text)
        except ValueError:
            return 0
        return 1

    def ThresholdInput(self,queryString):

        thresholdString = self.InputText(queryString,self.ThresholdValidator)

        threshold = None
        if thresholdString != 'n':
            threshold = float(thresholdString)

        return threshold

    def PrintProgress(self,obj,event):
        self.OutputProgress(obj.GetProgress(),10)

    def LevelSetEvolution(self):

        if self.LevelSetsType == "geodesic":
            levelSets = vtkvmtk.vtkvmtkGeodesicActiveContourLevelSetImageFilter()
            levelSets.SetFeatureImage(self.FeatureImage)
            levelSets.SetDerivativeSigma(self.FeatureDerivativeSigma)
            levelSets.SetAutoGenerateSpeedAdvection(1)
            levelSets.SetPropagationScaling(self.PropagationScaling)
            levelSets.SetCurvatureScaling(self.CurvatureScaling)
            levelSets.SetAdvectionScaling(self.AdvectionScaling)

        elif self.LevelSetsType == "curves":
            levelSets = vtkvmtk.vtkvmtkCurvesLevelSetImageFilter()
            levelSets.SetFeatureImage(self.FeatureImage)
            levelSets.SetDerivativeSigma(self.FeatureDerivativeSigma)
            levelSets.SetAutoGenerateSpeedAdvection(1)
            levelSets.SetPropagationScaling(self.PropagationScaling)
            levelSets.SetCurvatureScaling(self.CurvatureScaling)
            levelSets.SetAdvectionScaling(self.AdvectionScaling)

        elif self.LevelSetsType == "threshold":
            levelSets = vtkvmtk.vtkvmtkThresholdSegmentationLevelSetImageFilter()
            levelSets.SetFeatureImage(self.Image)
            queryString = "Please input lower threshold (\'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)
            queryString = "Please input upper threshold (\'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)
            scalarRange = self.Image.GetScalarRange()
            if self.LowerThreshold != None:
                levelSets.SetLowerThreshold(self.LowerThreshold)
            else:
                levelSets.SetLowerThreshold(scalarRange[0]-1.0)
            if self.UpperThreshold != None:
                levelSets.SetUpperThreshold(self.UpperThreshold)
            else:
                levelSets.SetUpperThreshold(scalarRange[1]+1.0)
            levelSets.SetEdgeWeight(self.EdgeWeight)
            levelSets.SetSmoothingIterations(self.SmoothingIterations)
            levelSets.SetSmoothingTimeStep(self.SmoothingTimeStep)
            levelSets.SetSmoothingConductance(self.SmoothingConductance)
            levelSets.SetPropagationScaling(self.PropagationScaling)
            levelSets.SetCurvatureScaling(self.CurvatureScaling)

        elif self.LevelSetsType == "laplacian":
            levelSets = vtkvmtk.vtkvmtkLaplacianSegmentationLevelSetImageFilter()
            levelSets.SetFeatureImage(self.Image)
            levelSets.SetPropagationScaling(-self.PropagationScaling)
            levelSets.SetCurvatureScaling(self.CurvatureScaling)

        else:
            self.PrintError('Unsupported LevelSetsType')

        levelSets.SetInputData(self.LevelSetsInput)
        levelSets.SetNumberOfIterations(self.NumberOfIterations)
        levelSets.SetIsoSurfaceValue(self.IsoSurfaceValue)
        levelSets.SetMaximumRMSError(self.MaximumRMSError)
        levelSets.SetInterpolateSurfaceLocation(1)
        levelSets.SetUseImageSpacing(1)
        levelSets.AddObserver("ProgressEvent", self.PrintProgress)
        levelSets.Update()

        self.EndProgress()

        self.LevelSetsOutput = vtk.vtkImageData()
        self.LevelSetsOutput.DeepCopy(levelSets.GetOutput())

    def MergeLevelSet(self):

        if self.LevelSets == None:
            self.LevelSets = self.LevelSetsOutput
        else:
            minFilter = vtk.vtkImageMathematics()
            minFilter.SetOperationToMin()
            minFilter.SetInput1Data(self.LevelSets)
            minFilter.SetInput2Data(self.LevelSetsOutput)
            minFilter.Update()
            self.LevelSets = minFilter.GetOutput()

    def DisplayLevelSetSurface(self,levelSets,value=0.0):

        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInputData(levelSets)
        marchingCubes.SetValue(0,value)
        marchingCubes.Update()

        self.OutputText('Displaying.\n')

        self.SurfaceViewer.Surface = marchingCubes.GetOutput()
        self.SurfaceViewer.Display = 0
        self.SurfaceViewer.Opacity = 0.5
        self.SurfaceViewer.BuildView()

    def YesNoValidator(self,text):
        if text in ['n','y']:
            return 1
        return 0

    def EvolutionParametersValidator(self,text):
        if not text:
            return 1
        if text in ['q','e']:
            return 1
        splitText = text.strip().split(' ')
        if len(splitText) not in [1,4]:
            return 0
        try:
            int(splitText[0])
            if len(splitText) == 4:
                float(splitText[1])
                float(splitText[2])
                float(splitText[3])
        except ValueError:
            return 0
        return 1

    def Execute(self):
        from vmtk import vmtkscripts
        if self.Image == None:
            self.PrintError('Error: no Image.')

        if self.Image.GetScalarTypeAsString() != 'float':
            cast = vtk.vtkImageCast()
            cast.SetInputData(self.Image)
            cast.SetOutputScalarTypeToFloat()
            cast.Update()
            self.Image = cast.GetOutput()

        if not self.InitializationImage:
            self.InitializationImage = self.Image

        if not self.FeatureImage:
            if self.LevelSetsType in ["geodesic", "curves"]:
                imageFeatures = vmtkscripts.vmtkImageFeatures()
                imageFeatures.Image = self.Image
                imageFeatures.FeatureImageType = self.FeatureImageType
                imageFeatures.SigmoidRemapping = self.SigmoidRemapping
                imageFeatures.DerivativeSigma = self.FeatureDerivativeSigma
                imageFeatures.UpwindFactor = self.UpwindFactor
                imageFeatures.FWHMRadius = self.FWHMRadius
                imageFeatures.FWHMBackgroundValue = self.FWHMBackgroundValue
                imageFeatures.Execute()
                self.FeatureImage = imageFeatures.FeatureImage
            elif self.LevelSetsType in ["threshold", "laplacian"]:
                self.FeatureImage = self.Image
            else:
                self.PrintError('Unsupported LevelSetsType')

        if self.NumberOfIterations != 0:
            self.LevelSetsInput = self.InitialLevelSets
            self.LevelSetEvolution()
            self.MergeLevelSet()
            return

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkscripts.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1

        self.vmtkRenderer.RegisterScript(self)

        self.ImageSeeder = vmtkscripts.vmtkImageSeeder()
        self.ImageSeeder.vmtkRenderer = self.vmtkRenderer
        #self.ImageSeeder.Image = self.Image
        self.ImageSeeder.Image = self.InitializationImage
        self.ImageSeeder.Display = 0
        self.ImageSeeder.Execute()
        ##self.ImageSeeder.Display = 1
        self.ImageSeeder.BuildView()

        self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer

        if self.LevelSets != None:
            self.DisplayLevelSetSurface(self.LevelSets,0.0)

        self.vmtkImageInitialization = vmtkscripts.vmtkImageInitialization()
        #self.vmtkImageInitialization.Image = self.Image
        self.vmtkImageInitialization.Image = self.InitializationImage
        self.vmtkImageInitialization.vmtkRenderer = self.vmtkRenderer
        self.vmtkImageInitialization.ImageSeeder = self.ImageSeeder
        self.vmtkImageInitialization.SurfaceViewer = self.SurfaceViewer
        self.vmtkImageInitialization.NegateImage = self.NegateForInitialization
        self.vmtkImageInitialization.OwnRenderer = 0

        endSegmentation = 0
        while (endSegmentation == 0):

            if self.InitialLevelSets == None:
                self.vmtkImageInitialization.Execute()
                self.LevelSetsInput = self.vmtkImageInitialization.InitialLevelSets
#                self.IsoSurfaceValue = self.vmtkImageInitialization.IsoSurfaceValue
                self.vmtkImageInitialization.InitialLevelSets = None
#                self.vmtkImageInitialization.IsosurfaceValue = 0.0
                self.IsoSurfaceValue = 0.0
            else:
                self.LevelSetsInput = self.InitialLevelSets
                self.InitialLevelSets = None
                self.DisplayLevelSetSurface(self.LevelSetsInput,self.IsoSurfaceValue)

            endEvolution = False
            while not endEvolution:

                queryString = 'Please input parameters (type return to accept current values, \'e\' to end, \'q\' to quit):\nNumberOfIterations('+str(self.NumberOfIterations)+') [PropagationScaling('+str(self.PropagationScaling)+') CurvatureScaling('+str(self.CurvatureScaling)+') AdvectionScaling('+str(self.AdvectionScaling)+')]: '
                inputString = self.InputText(queryString,self.EvolutionParametersValidator)

                if inputString == 'q':
                    return
                elif inputString == 'e':
                    endEvolution = True
                elif inputString != '':
                    splitInputString = inputString.strip().split(' ')
                    if len(splitInputString) == 1:
                        self.NumberOfIterations = int(splitInputString[0])
                    elif len(splitInputString) == 4:
                        self.NumberOfIterations = int(splitInputString[0])
                        self.PropagationScaling = float(splitInputString[1])
                        self.CurvatureScaling = float(splitInputString[2])
                        self.AdvectionScaling = float(splitInputString[3])
                    else:
                        self.PrintLog('Wrong number of parameters.')
                        continue

                if endEvolution:
                    break

                self.LevelSetEvolution()
                self.DisplayLevelSetSurface(self.LevelSetsOutput)

                queryString = 'Accept result? (y/n): '
                inputString = self.InputText(queryString,self.YesNoValidator)
                if inputString == 'y':
                    endEvolution = True
                elif inputString == 'n':
                    endEvolution = False

            queryString = 'Merge branch? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if inputString == 'y':
                self.MergeLevelSet()
            elif inputString == 'n':
                pass

            if self.LevelSets != None:
                self.DisplayLevelSetSurface(self.LevelSets)

            queryString = 'Segment another branch? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if inputString == 'y':
                endSegmentation = False
            elif inputString == 'n':
                endSegmentation = True
        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
