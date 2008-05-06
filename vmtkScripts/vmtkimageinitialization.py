#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageinitialization.py,v $
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

vmtkimageinitialization = 'vmtkImageInitialization'

class vmtkImageInitialization(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.InitialLevelSets = None
        self.Surface = None

        self.MergedInitialLevelSets = None

        self.UpperThreshold = 0.0
        self.LowerThreshold = 0.0

        self.IsoSurfaceValue = 0.0

        self.ImageSeeder = None
        self.SurfaceViewer = None

        self.SetScriptName('vmtkimageinitialization')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','','vmtkimagereader'],
            ['vmtkRenderer','renderer','vmtkRenderer',1]
            ])
        self.SetOutputMembers([
            ['IsoSurfaceValue','isosurfacevalue','float',1],
            ['InitialLevelSets','olevelsets','vtkImageData',1,'','','vmtkimagewriter'],
            ['Surface','osurface','vtkPolyData',1,'','','vmtksurfacewriter']
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
        
        imageMathematics = vtk.vtkImageMathematics()
        imageMathematics.SetInput(self.Image)
        imageMathematics.SetConstantK(-1.0)
        imageMathematics.SetOperationToMultiplyByK()
        imageMathematics.Update()

        subtract = vtk.vtkImageMathematics()
        subtract.SetInput(imageMathematics.GetOutput())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(self.IsoSurfaceValue)
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())
        self.InitialLevelSets.Update()

        self.IsoSurfaceValue = 0.0

    def ThresholdInitialize(self):

        self.PrintLog('Threshold initialization.')

        queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
        self.LowerThreshold = self.ThresholdInput(queryString)

        queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
        self.UpperThreshold = self.ThresholdInput(queryString)

        scalarRange = self.Image.GetScalarRange()

        thresholdedImage = self.Image
	
        if self.LowerThreshold != None or self.UpperThreshold != None:
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(self.Image)
            if self.LowerThreshold != None and self.UpperThreshold != None:
                threshold.ThresholdBetween(self.LowerThreshold,self.UpperThreshold)
            elif self.LowerThreshold != None:
                threshold.ThresholdByUpper(self.LowerThreshold)
            elif self.UpperThreshold != None:
                threshold.ThresholdByLower(self.UpperThreshold)
            threshold.ReplaceInOn()
            threshold.ReplaceOutOn()
            threshold.SetInValue(-1.0)
            threshold.SetOutValue(1.0)
            threshold.Update()
        
            thresholdedImage = threshold.GetOutput()
 
        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(thresholdedImage)
        self.InitialLevelSets.Update()

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

        scalarRange = self.Image.GetScalarRange()

        thresholdedImage = self.Image
	
        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(self.Image)
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
        fastMarching.GenerateGradientImageOff()
        fastMarching.SetTargetOffset(100.0)
        if targetSeedIds.GetNumberOfIds() > 0:
            fastMarching.SetTargets(targetSeedIds)
            fastMarching.SetTargetReachedModeToOneTarget()
        else:
            fastMarching.SetTargetReachedModeToNoTargets()
        fastMarching.Update()

        subtract = vtk.vtkImageMathematics()
        subtract.SetInput(fastMarching.GetOutput())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(-fastMarching.GetTargetValue())
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())
        self.InitialLevelSets.Update()

        self.IsoSurfaceValue = 0.0

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

        scalarRange = self.Image.GetScalarRange()
	
        thresholdedImage = self.Image

        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInput(self.Image)
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

        subtract = vtk.vtkImageMathematics()
        subtract.SetInput(collidingFronts.GetOutput())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(-10.0 * collidingFronts.GetNegativeEpsilon())
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())
        self.InitialLevelSets.Update()

        self.IsoSurfaceValue = 0.0 

    def SeedInitialize(self):

        self.PrintLog('Seed initialization.')

        queryString = 'Please place seeds'
        seeds = self.SeedInput(queryString,0)
        
        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(self.Image)
        self.InitialLevelSets.Update()

        levelSetsInputScalars = self.InitialLevelSets.GetPointData().GetScalars()
        levelSetsInputScalars.FillComponent(0,1.0)

        dimensions = self.Image.GetDimensions()
        for i in range(seeds.GetNumberOfPoints()):
            id = self.Image.FindPoint(seeds.GetPoint(i))
            levelSetsInputScalars.SetComponent(id,0,-1.0)

        dilateErode = vtk.vtkImageDilateErode3D()
        dilateErode.SetInput(self.InitialLevelSets)
        dilateErode.SetDilateValue(-1.0)
        dilateErode.SetErodeValue(1.0)
        dilateErode.SetKernelSize(3,3,3)
        dilateErode.Update()

        self.InitialLevelSets.DeepCopy(dilateErode.GetOutput())

        self.IsoSurfaceValue = 0.0

    def DisplayLevelSetSurface(self,levelSets):
     
        value = 0.0 
        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInput(levelSets)
        marchingCubes.SetValue(0,value)
        marchingCubes.Update()

        self.Surface = marchingCubes.GetOutput()

        self.OutputText('Displaying.\n')
  
        self.SurfaceViewer.Surface = marchingCubes.GetOutput()
        if self.SurfaceViewer.Surface.GetSource():
            self.SurfaceViewer.Surface.GetSource().UnRegisterAllOutputs()
        self.SurfaceViewer.Display = 1
        self.SurfaceViewer.Opacity = 0.5
        self.SurfaceViewer.BuildView()

    def InitializationTypeValidator(self,text):
        if text in ['0','1','2','3','4']:
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
            if char not in string.digits + '.' + ' ' + '-':
                return 0
        if len(text.strip().split(' ')) in [1,4]:
            return 1
        return 0

    def MergeLevelSets(self):

        if self.MergedInitialLevelSets == None:
            self.MergedInitialLevelSets = vtk.vtkImageData()
            self.MergedInitialLevelSets.DeepCopy(self.InitialLevelSets)
        else:
            minFilter = vtk.vtkImageMathematics()
            minFilter.SetOperationToMin()
            minFilter.SetInput1(self.MergedInitialLevelSets)
            minFilter.SetInput2(self.InitialLevelSets)
            minFilter.Update()
            self.MergedInitialLevelSets = minFilter.GetOutput()

    def Execute(self):
          
        if self.Image == None:
            self.PrintError('Error: no Image.')

        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
        self.Image = cast.GetOutput()

        if not self.vmtkRenderer:
            self.vmtkRenderer = vmtkscripts.vmtkRenderer()
            self.vmtkRenderer.Initialize()
            self.OwnRenderer = 1
 
        if not self.ImageSeeder: 
            self.ImageSeeder = vmtkscripts.vmtkImageSeeder()
            self.ImageSeeder.vmtkRenderer = self.vmtkRenderer
            self.ImageSeeder.Image = self.Image
            self.ImageSeeder.Display = 0
            self.ImageSeeder.Execute()
            self.ImageSeeder.Display = 1
            self.ImageSeeder.BuildView()
  
        if not self.SurfaceViewer:
            self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
            self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer
  
        initializationMethods = {
                                '0': self.CollidingFrontsInitialize,
                                '1': self.FastMarchingInitialize,
                                '2': self.ThresholdInitialize,
                                '3': self.IsosurfaceInitialize,
                                '4': self.SeedInitialize
                                }
  
        endInitialization = False
        while not endInitialization:
            queryString = 'Please choose initialization type: (0: colliding fronts; 1: fast marching; 2: threshold; 3: isosurface, 4: seed): '
            initializationType = self.InputText(queryString,self.InitializationTypeValidator)
            initializationMethods[initializationType]()
            self.DisplayLevelSetSurface(self.InitialLevelSets)
            queryString = 'Accept initialization? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if inputString == 'y':
                self.MergeLevelSets()
                self.DisplayLevelSetSurface(self.MergedInitialLevelSets)
            queryString = 'Initialize another branch? (y/n): '
            inputString = self.InputText(queryString,self.YesNoValidator)
            if inputString == 'y':
                endInitialization = False
            elif inputString == 'n':
                endInitialization = True

        self.InitialLevelSets = self.MergedInitialLevelSets

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
