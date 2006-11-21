#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtklevelsetsegmentation.py,v $
## Language:  Python
## Date:      $Date: 2006/05/31 10:51:21 $
## Version:   $Revision: 1.19 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import sys
import math
import string
import vtk
import vtkvmtk
import vmtkscripts

import pypes

vmtklevelsetsegmentation = 'vmtkLevelSetSegmentation'

class vmtkLevelSetSegmentation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.InitialLevelSets = None
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
        self.MaxvmtkmRMSError = 1E-20
        self.DerivativeSigma = 0.0
        self.FeatureDerivativeSigma = 0.0

        self.SigmoidRemapping = 0

        self.FeatureImageType = 'gradient'
        
        self.UpwindFactor = 1.0
	
        self.FWHMRadius = [1.0, 1.0, 1.0]
        self.FWHMBackgroundValue = 0.0
        
        self.ResampleImage = 1
        self.ResampleSpacing = 0.0

        self.ImageSeeder = None

        self.SetScriptName('vmtklevelsetsegmentation')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','vmtkimagereader'],
            ['FeatureImage','featureimage','vtkImageData',1,'','vmtkimagereader'],
            ['InitialLevelSets','initiallevelsets','vtkImageData',1,'','vmtkimagereader'],
            ['LevelSets','levelsets','vtkImageData',1,'','vmtkimagereader'],
            ['FeatureImageType','featureimagetype','str',1],
            ['SigmoidRemapping','sigmoid','int',1],
            ['IsoSurfaceValue','isosurfacevalue','float',1],
            ['DerivativeSigma','derivativesigma','float',1],
            ['FeatureDerivativeSigma','featurederivativesigma','float',1],
            ['ResampleImage','resampleimage','int',1],
            ['ResampleSpacing','resamplespacing','float',1],
            ['UpwindFactor','upwindfactor','float',1],
            ['FWHMRadius','fwhmradius','float',3],
            ['FWHMBackgroundValue','fwhmbackgroundvalue','float',1],
            ['NumberOfIterations','iterations','int',1],
            ['PropagationScaling','propagation','float',1],
            ['CurvatureScaling','curvature','float',1],
            ['AdvectionScaling','advection','float',1],
            ['vmtkRenderer','renderer','vmtkRenderer',1]
            ])
        self.SetOutputMembers([
            ['LevelSets','o','vtkImageData',1,'','vmtkimagewriter'],
            ['FeatureImage','featureimage','vtkImageData',1,'','vmtkimagewriter']
            ])

    def SeedInput(self,queryString,numberOfSeeds):

        invalid = 1
        while invalid == 1:
            invalid = 0

            self.OutputText(queryString+' (click on the image while pressing Ctrl).\n')
            self.ImageSeeder.InitializeSeeds()
            self.vmtkRenderer.Render()

            if numberOfSeeds > 0:
                if self.ImageSeeder.Seeds.GetNumberOfPoints() != numberOfSeeds:
                    self.OutputText('Invalid selection. Please place exactly '+str(numberOfSeeds)+' seeds.\n')
                    invalid = 1
                    continue

        seeds = vtk.vtkPolyData()
        seeds.DeepCopy(self.ImageSeeder.Seeds)
        seeds.Update()

        return seeds

    def ThresholdValidator(self,text):
        if text == 'n':
            return 1
        if text == 'i':
            self.vmtkRenderer.Render()
            return 0
        for char in text:
            if char not in string.digits + '.' +  ' ' + '-':
                return 0
        return 1

    def ThresholdInput(self,queryString):
       
        thresholdString = self.InputText(queryString,self.ThresholdValidator)
      
        threshold = None
        if thresholdString != 'n':
            threshold = float(thresholdString)
       
        return threshold
       
    def IsosurfaceInitialize(self):

        self.PrintLog('Isosurface initialization.')

        queryString = "Please input isosurface level (\'i\' to activate image, \'n\' for none): "
        self.IsoSurfaceValue = self.ThresholdInput(queryString)
        
        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()

        imageMathematics = vtk.vtkImageMathematics()
        imageMathematics.SetInput(cast.GetOutput())
        imageMathematics.SetConstantK(-1.0)
        imageMathematics.SetOperationToMultiplyByK()
        imageMathematics.Update()

        self.IsoSurfaceValue *= -1.0

        self.LevelSetsInput = vtk.vtkImageData()
        self.LevelSetsInput.DeepCopy(imageMathematics.GetOutput())
        self.LevelSetsInput.Update()

    def ThresholdInitialize(self):

        self.PrintLog('Threshold initialization.')

        queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
        self.LowerThreshold = self.ThresholdInput(queryString)

        queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
        self.UpperThreshold = self.ThresholdInput(queryString)

        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
                                                                
        scalarRange = cast.GetOutput().GetScalarRange()

        thresholdedImage = cast.GetOutput()
	
        if self.LowerThreshold or self.UpperThreshold:
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(cast.GetOutput())
            if self.LowerThreshold and self.UpperThreshold:
                threshold.ThresholdBetween(self.LowerThreshold,self.UpperThreshold)
            elif self.LowerThreshold:
                threshold.ThresholdByUpper(self.LowerThreshold)
            elif self.UpperThreshold:
                threshold.ThresholdByLower(self.UpperThreshold)
            threshold.ReplaceInOn()
            threshold.ReplaceOutOn()
            threshold.SetInValue(-1.0)
            threshold.SetOutValue(1.0)
            threshold.Update()
        
            thresholdedImage = threshold.GetOutput()
 
        self.LevelSetsInput = vtk.vtkImageData()
        self.LevelSetsInput.DeepCopy(thresholdedImage)
        self.LevelSetsInput.Update()

        self.IsoSurfaceValue = 0.0

    def FastMarchingInitialize(self):

        self.PrintLog('Fast marching initialization.')

        queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
        self.LowerThreshold = self.ThresholdInput(queryString)

        queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
        self.UpperThreshold = self.ThresholdInput(queryString)

        queryString = 'Please place source seeds'
        sourceSeeds = self.SeedInput(queryString,0)
        
        queryString = 'Please place target seeds'
        targetSeeds = self.SeedInput(queryString,0)

        sourceSeedIds = vtk.vtkIdList()
        for i in range(sourceSeeds.GetNumberOfPoints()):
            sourceSeedIds.InsertNextId(self.Image.FindPoint(sourceSeeds.GetPoint(i)))

        targetSeedIds = vtk.vtkIdList()
        for i in range(targetSeeds.GetNumberOfPoints()):
            targetSeedIds.InsertNextId(self.Image.FindPoint(targetSeeds.GetPoint(i)))

        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
                                                                
        scalarRange = cast.GetOutput().GetScalarRange()

        thresholdedImage = cast.GetOutput()
	
        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(cast.GetOutput())
            if (self.LowerThreshold is not None) & (self.UpperThreshold is not None):
                threshold.ThresholdBetween(self.LowerThreshold,self.UpperThreshold)
            elif (self.LowerThreshold is not None):
                threshold.ThresholdByUpper(self.LowerThreshold)
            elif (self.UpperThreshold is not None):
                threshold.ThresholdByLower(self.UpperThreshold)
            threshold.ReplaceInOff()
            threshold.ReplaceOutOn()
            threshold.SetOutValue(scalarRange[0] - scalarRange[1])
            threshold.Update()
        
            scalarRange = threshold.GetOutput().GetScalarRange()

            thresholdedImage = threshold.GetOutput()

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInput(thresholdedImage)
        shiftScale.SetShift(-scalarRange[0])
        shiftScale.SetScale(1/(scalarRange[1]-scalarRange[0]))
        shiftScale.Update()
        
        speedImage = shiftScale.GetOutput()

        fastMarching = vtkvmtk.vtkvmtkFastMarchingUpwindGradientImageFilter()
        fastMarching.SetInput(speedImage)
        fastMarching.SetSeeds(sourceSeedIds)
        fastMarching.SetTargets(targetSeedIds)
        fastMarching.GenerateGradientImageOff()
        fastMarching.SetTargetOffset(100.0)
        fastMarching.SetTargetReachedModeToOneTarget()
##         fastMarching.SetTargetReachedModeToAllTargets()
        fastMarching.Update()

        self.LevelSetsInput = vtk.vtkImageData()
        self.LevelSetsInput.DeepCopy(fastMarching.GetOutput())
        self.LevelSetsInput.Update()

        self.IsoSurfaceValue = fastMarching.GetTargetValue()

    def CollidingFrontsInitialize(self):

        self.PrintLog('Colliding fronts initialization.')

        queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
        self.LowerThreshold = self.ThresholdInput(queryString)

        queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
        self.UpperThreshold = self.ThresholdInput(queryString)

        queryString = 'Please place two seeds'
        seeds = self.SeedInput(queryString,2)

        seedIds1 = vtk.vtkIdList()
        seedIds2 = vtk.vtkIdList()

        seedIds1.InsertNextId(self.Image.FindPoint(seeds.GetPoint(0)))
        seedIds2.InsertNextId(self.Image.FindPoint(seeds.GetPoint(1)))

        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
        
        scalarRange = cast.GetOutput().GetScalarRange()
	
        thresholdedImage = cast.GetOutput()

        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(cast.GetOutput())
            if (self.LowerThreshold is not None) & (self.UpperThreshold is not None):
                threshold.ThresholdBetween(self.LowerThreshold,self.UpperThreshold)
            elif (self.LowerThreshold is not None):
                threshold.ThresholdByUpper(self.LowerThreshold)
            elif (self.UpperThreshold is not None):
                threshold.ThresholdByLower(self.UpperThreshold)
            threshold.ReplaceInOff()
            threshold.ReplaceOutOn()
            threshold.SetOutValue(scalarRange[0] - scalarRange[1])
            threshold.Update()
        
            scalarRange = threshold.GetOutput().GetScalarRange()

            thresholdedImage = threshold.GetOutput()

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInput(thresholdedImage)
        shiftScale.SetShift(-scalarRange[0])
        shiftScale.SetScale(1/(scalarRange[1]-scalarRange[0]))
        shiftScale.Update()
        
        speedImage = shiftScale.GetOutput()

        collidingFronts = vtkvmtk.vtkvmtkCollidingFrontsImageFilter()
        collidingFronts.SetInput(speedImage)
        collidingFronts.SetSeeds1(seedIds1)
        collidingFronts.SetSeeds2(seedIds2)
        collidingFronts.ApplyConnectivityOn()
        collidingFronts.Update()

        self.LevelSetsInput = vtk.vtkImageData()
        self.LevelSetsInput.DeepCopy(collidingFronts.GetOutput())
        self.LevelSetsInput.Update()

        self.IsoSurfaceValue = 10.0 * collidingFronts.GetNegativeEpsilon()

        return 1

    def BuildFWHMBasedFeatureImage(self):
        
        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()

        fwhmFeatureImageFilter = vtkvmtk.vtkvmtkFWHMFeatureImageFilter()
        fwhmFeatureImageFilter.SetInput(cast.GetOutput())
        fwhmFeatureImageFilter.SetRadius(self.FWHMRadius)
        fwhmFeatureImageFilter.SetBackgroundValue(self.FWHMBackgroundValue)
        fwhmFeatureImageFilter.Update()
	
        self.FeatureImage = vtk.vtkImageData()
        self.FeatureImage.DeepCopy(fwhmFeatureImageFilter.GetOutput())
        self.FeatureImage.Update()

    def BuildUpwindGradientBasedFeatureImage(self):
 
        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
       
        gradientMagnitude = vtkvmtk.vtkvmtkUpwindGradientMagnitudeImageFilter()
        gradientMagnitude.SetInput(cast.GetOutput())
        gradientMagnitude.SetUpwindFactor(self.UpwindFactor)
        gradientMagnitude.Update()

        featureImage = None
        if self.SigmoidRemapping==1:
            scalarRange = gradientMagnitude.GetOutput().GetPointData().GetScalars().GetRange()
            inputMinvmtkm = scalarRange[0]
            inputMaxvmtkm = scalarRange[1]
            alpha = - (inputMaxvmtkm - inputMinvmtkm) / 6.0
            beta = (inputMaxvmtkm + inputMinvmtkm) / 2.0
            sigmoid = vtkvmtk.vtkvmtkSigmoidImageFilter()
            sigmoid.SetInput(gradientMagnitude.GetOutput())
            sigmoid.SetAlpha(alpha)
            sigmoid.SetBeta(beta)
            sigmoid.SetOutputMinvmtkm(0.0)
            sigmoid.SetOutputMaxvmtkm(1.0)
            sigmoid.Update()
            featureImage = sigmoid.GetOutput()
        else:
            boundedReciprocal = vtkvmtk.vtkvmtkBoundedReciprocalImageFilter()
            boundedReciprocal.SetInput(gradientMagnitude.GetOutput())
            boundedReciprocal.Update()
            featureImage = boundedReciprocal.GetOutput()
 
        self.FeatureImage = vtk.vtkImageData()
        self.FeatureImage.DeepCopy(featureImage)
        self.FeatureImage.Update()
   
    def BuildGradientBasedFeatureImage(self):

        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()

        if (self.DerivativeSigma > 0.0):
            gradientMagnitude = vtkvmtk.vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter()
            gradientMagnitude.SetInput(cast.GetOutput())
            gradientMagnitude.SetSigma(self.DerivativeSigma)
            gradientMagnitude.SetNormalizeAcrossScale(0)
            gradientMagnitude.Update()
        else:
            gradientMagnitude = vtkvmtk.vtkvmtkGradientMagnitudeImageFilter()
            gradientMagnitude.SetInput(cast.GetOutput())
            gradientMagnitude.Update()

        featureImage = None
        if self.SigmoidRemapping==1:
            scalarRange = gradientMagnitude.GetOutput().GetPointData().GetScalars().GetRange()
            inputMinvmtkm = scalarRange[0]
            inputMaxvmtkm = scalarRange[1]
            alpha = - (inputMaxvmtkm - inputMinvmtkm) / 6.0
            beta = (inputMaxvmtkm + inputMinvmtkm) / 2.0
            sigmoid = vtkvmtk.vtkvmtkSigmoidImageFilter()
            sigmoid.SetInput(gradientMagnitude.GetOutput())
            sigmoid.SetAlpha(alpha)
            sigmoid.SetBeta(beta)
            sigmoid.SetOutputMinvmtkm(0.0)
            sigmoid.SetOutputMaxvmtkm(1.0)
            sigmoid.Update()
            featureImage = sigmoid.GetOutput()
        else:
            boundedReciprocal = vtkvmtk.vtkvmtkBoundedReciprocalImageFilter()
            boundedReciprocal.SetInput(gradientMagnitude.GetOutput())
            boundedReciprocal.Update()
            featureImage = boundedReciprocal.GetOutput()
 
        self.FeatureImage = vtk.vtkImageData()
        self.FeatureImage.DeepCopy(featureImage)
        self.FeatureImage.Update()

    def PrintProgress(self,obj,event):
        self.OutputProgress(obj.GetProgress(),10)

    def LevelSetEvolution(self):

        levelSets = vtkvmtk.vtkvmtkGeodesicActiveContourLevelSetImageFilter()
##        levelSets = vtkvmtk.vtkvmtkCurvesLevelSetImageFilter()
        levelSets.SetDerivativeSigma(self.FeatureDerivativeSigma)
        levelSets.SetInput(self.LevelSetsInput)
        levelSets.SetFeatureImage(self.FeatureImage)
        levelSets.SetAutoGenerateSpeedAdvection(1)
        levelSets.SetNumberOfIterations(self.NumberOfIterations)
        levelSets.SetPropagationScaling(self.PropagationScaling)
        levelSets.SetCurvatureScaling(self.CurvatureScaling)
        levelSets.SetAdvectionScaling(self.AdvectionScaling)
        levelSets.SetIsoSurfaceValue(self.IsoSurfaceValue)
        levelSets.SetMaxvmtkmRMSError(self.MaxvmtkmRMSError)
        levelSets.SetInterpolateSurfaceLocation(1)
        levelSets.AddObserver("ProgressEvent", self.PrintProgress)
        levelSets.Update()

        self.EndProgress()
  
        self.LevelSetsOutput = vtk.vtkImageData()
        self.LevelSetsOutput.DeepCopy(levelSets.GetOutput())
        self.LevelSetsOutput.Update()

        if self.LevelSetsOutput.GetSource():
            self.LevelSetsOutput.GetSource().UnRegisterAllOutputs()

    def MergeLevelSet(self):

        if self.LevelSets == None:
            self.LevelSets = self.LevelSetsOutput
        else:
            minvmtkm = vtk.vtkImageMathematics()
            minvmtkm.SetOperationToMin()
            minvmtkm.SetInput1(self.LevelSets)
            minvmtkm.SetInput2(self.LevelSetsOutput)
            minvmtkm.Update()
            self.LevelSets = minvmtkm.GetOutput()

        if self.LevelSets.GetSource():
            self.LevelSets.GetSource().UnRegisterAllOutputs()

    def DisplayLevelSetSurface(self,levelSets,value=0.0):
      
        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInput(levelSets)
        marchingCubes.SetValue(0,value)
        marchingCubes.Update()

        self.OutputText('Displaying.\n')
  
        self.SurfaceViewer.Surface = marchingCubes.GetOutput()
        if self.SurfaceViewer.Surface.GetSource():
            self.SurfaceViewer.Surface.GetSource().UnRegisterAllOutputs()
        self.SurfaceViewer.Display = 1
        self.SurfaceViewer.Opacity = 0.5
        self.SurfaceViewer.BuildView()

    def MakeImageIsotropic(self):
        
        spacing = self.Image.GetSpacing()
        if (spacing[0] == spacing[1]) & (spacing[1] == spacing[2]) & (self.ResampleSpacing == 0.0):
            return

        isotropicSpacing = spacing[0]
        if self.ResampleSpacing != 0.0:
            isotropicSpacing = self.ResampleSpacing

        reslice = vtk.vtkImageReslice()
        reslice.SetInput(self.Image)
        reslice.SetBackgroundLevel(self.Image.GetPointData().GetScalars().GetValue(0))
        reslice.SetInterpolationModeToCubic()
        reslice.InterpolateOn()
        reslice.SetOutputSpacing(isotropicSpacing,isotropicSpacing,isotropicSpacing)
        reslice.Update()
        self.Image = reslice.GetOutput()

        if self.InitialLevelSets != None:
          reslice2 = vtk.vtkImageReslice()
          reslice2.SetInput(self.InitialLevelSets)
          reslice2.SetBackgroundLevel(self.InitialLevelSets.GetPointData().GetScalars().GetValue(0))
          reslice2.SetInterpolationModeToLinear()
          reslice2.InterpolateOn()
          reslice2.SetOutputSpacing(isotropicSpacing,isotropicSpacing,isotropicSpacing)
          reslice2.Update()
          self.InitialLevelSets = reslice2.GetOutput()

##        reslice = vtk.vtkImageResample()
##        reslice.SetInput(self.Image)
##        reslice.SetBackgroundLevel(self.Image.GetPointData().GetScalars().GetValue(0))
##        reslice.SetInterpolationModeToCubic()
##        reslice.InterpolateOn()
##        reslice.SetAxisOutputSpacing(0,isotropicSpacing)
##        reslice.SetAxisOutputSpacing(1,isotropicSpacing)
##        reslice.SetAxisOutputSpacing(2,isotropicSpacing)
##        reslice.SetDimensionality(3)
##        reslice.Update()
##        self.Image = reslice.GetOutput()

##        if self.Image.GetSource():
##            self.Image.GetSource().UnRegisterAllOutputs()

    def InitializationTypeValidator(self,text):
        if text in ['0','1','2','3']:
            return 1
        return 0

    def YesNoValidator(self,text):
        if text in ['n','y']:
            return 1
        return 0

    def EvolutionParametersValidator(self,text):
        if not text:
            return 1
        if text in ['q','e']:
            return 1
        for char in text:
            if char not in string.digits + '.' + ' ':
                return 0
        if len(text.strip().split(' ')) in [1,4]:
            return 1
        return 0

    def Execute(self):
          
        if self.Image == None:
            self.PrintError('Error: no Image.')

        if self.ResampleImage==1:
            self.MakeImageIsotropic()

        if self.FeatureImageType == 'gradient':
          self.BuildGradientBasedFeatureImage()
        elif self.FeatureImageType == 'upwind':
          self.BuildUpwindGradientBasedFeatureImage()
        elif self.FeatureImageType == 'fwhm':
          self.BuildFWHMBasedFeatureImage()
        else:
          self.PrintError('Error: unsupported feature image type')

        if self.NumberOfIterations != 0:
            self.LevelSetsInput = self.InitialLevelSets
            self.LevelSetEvolution()
            self.MergeLevelSet()
            return

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkscripts.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1
  
        self.ImageSeeder = vmtkscripts.vmtkImageSeeder()
        self.ImageSeeder.vmtkRenderer = self.vmtkRenderer
        self.ImageSeeder.Image = self.Image
        self.ImageSeeder.Display = 0
        self.ImageSeeder.Execute()
        self.ImageSeeder.Display = 1
        self.ImageSeeder.BuildView()
  
        self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
        self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer
  
        if self.LevelSets != None:
            self.DisplayLevelSetSurface(self.LevelSets,0.0)
  
        initializationMethods = {
                                '0': self.CollidingFrontsInitialize,
                                '1': self.FastMarchingInitialize,
                                '2': self.ThresholdInitialize,
                                '3': self.IsosurfaceInitialize
                                }
  
        endSegmentation = 0
        while (endSegmentation == 0):
  
            if self.InitialLevelSets == None:
              endInitialization = 0
              while (endInitialization == 0):
                queryString = 'Please choose initialization type: (0: colliding fronts; 1: fast marching; 2: threshold; 3: isosurface): '
                initializationType = self.InputText(queryString,self.InitializationTypeValidator)
                initializationMethods[initializationType]()
                self.DisplayLevelSetSurface(self.LevelSetsInput,self.IsoSurfaceValue)
                queryString = 'Accept initialization? (y/n): '
                inputString = self.InputText(queryString,self.YesNoValidator)
                if (inputString == 'y'):
                  endInitialization = 1
                elif (inputString == 'n'):
                  endInitialization = 0
            else:
              self.LevelSetsInput = self.InitialLevelSets
              self.InitialLevelSets = None
              self.DisplayLevelSetSurface(self.LevelSetsInput,self.IsoSurfaceValue)
  
            endEvolution = 0
            while (endEvolution == 0):
  
                queryString = 'Please input parameters (type return to accept current values, \'e\' to end, \'q\' to quit):\nNumberOfIterations('+str(self.NumberOfIterations)+') [PropagationScaling('+str(self.PropagationScaling)+') CurvatureScaling('+str(self.CurvatureScaling)+') AdvectionScaling('+str(self.AdvectionScaling)+')]: '
                inputString = self.InputText(queryString,self.EvolutionParametersValidator)
  
                if inputString == 'q':
                    return
                elif inputString == 'e':
                    endEvolution = 1
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
                if (inputString == 'y'):
                    endEvolution = 1
                elif (inputString == 'n'):
                    endEvolution = 0
  
            queryString = 'Merge branch? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if (inputString == 'y'):
                self.MergeLevelSet()
            elif (inputString == 'n'):
                pass
  
            self.DisplayLevelSetSurface(self.LevelSets)
  
            queryString = 'Segment another branch? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if (inputString == 'y'):
                endSegmentation = 0
            elif (inputString == 'n'):
                endSegmentation = 1
        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()
  

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
