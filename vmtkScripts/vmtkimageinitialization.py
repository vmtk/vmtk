#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkimageinitialization.py,v $
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
from random import random

from vmtk import vtkvmtk

from vmtk import pypes


class vmtkImageInitialization(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Image = None

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.InitialLevelSets = None
        self.Surface = None

        self.MergedInitialLevelSets = None

        self.Interactive = 1
        self.Method = "collidingfronts"

        self.UpperThreshold = None
        self.LowerThreshold = None

        self.SourcePoints = []
        self.TargetPoints = []

        self.NegateImage = 0
        self.NumSeedsSelected = 0

        self.IsoSurfaceValue = 0.0

        self.ImageSeeder = None
        self.SurfaceViewer = None

        self.SetScriptName('vmtkimageinitialization')
        self.SetScriptDoc('interactivly create an initial levelset which has not been evolved to image gradients')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','','vmtkimagereader'],
            ['NegateImage','negate','bool',1,'','negate image values before initializing'],
            ['Interactive','interactive','bool',1,'',''],
            ['Method','method','str',1,'["isosurface","threshold","collidingfronts","fastmarching","seeds","multiplecollidingfronts"]',''],
            ['SourcePoints','sourcepoints','int',-1,'','list of source point IJK coordinates'],
            ['TargetPoints','targetpoints','int',-1,'','list of target point IJK coordinates'],
            ['UpperThreshold','upperthreshold','float',1,'','the value of the upper threshold to use for threshold, collidingfronts and fastmarching'],
            ['LowerThreshold','lowerthreshold','float',1,'','the value of the upper threshold to use for threshold, collidingfronts and fastmarching'],
            ['IsoSurfaceValue','isosurface','float',1,'','the isosurface value to adopt as the level set surface'],
            ['vmtkRenderer','renderer','vmtkRenderer',1]
            ])
        self.SetOutputMembers([
            ['IsoSurfaceValue','isosurfacevalue','float',1],
            ['InitialLevelSets','olevelsets','vtkImageData',1,'','','vmtkimagewriter'],
            ['Surface','osurface','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def _SeedPickCallback(self, caller, event):
        '''change seed rgb color when two seeds have been selected'''
        self.NumSeedsSelected = self.ImageSeeder.Seeds.GetNumberOfPoints()
        if self.NumSeedsSelected % 2 == 0:
            self.ImageSeeder.SeedRGBColor = [random(), random(), random()]
            self.ImageSeeder.BuildView()

    def SeedInput(self,queryString,numberOfSeeds):
        invalid = 1
        while invalid == 1:
            invalid = 0
            self.InputInfo(queryString+' (click on the image while pressing Ctrl).\n')
            self.ImageSeeder.InitializeSeeds()
            self.vmtkRenderer.Render()

            if numberOfSeeds > 0:
                if self.ImageSeeder.Seeds.GetNumberOfPoints() != numberOfSeeds:
                    self.InputInfo('Invalid selection. Please place exactly '+str(numberOfSeeds)+' seeds.\n')
                    invalid = 1
                    continue

        seeds = vtk.vtkPolyData()
        seeds.DeepCopy(self.ImageSeeder.Seeds)

        return seeds

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

    def IsosurfaceInitialize(self):

        self.PrintLog('Isosurface initialization.')

        if self.Interactive:
            queryString = "Please input isosurface level (\'n\' for none): "
            self.IsoSurfaceValue = self.ThresholdInput(queryString)

        imageMathematics = vtk.vtkImageMathematics()
        imageMathematics.SetInputData(self.Image)
        imageMathematics.SetConstantK(-1.0)
        imageMathematics.SetOperationToMultiplyByK()
        imageMathematics.Update()

        subtract = vtk.vtkImageMathematics()
        subtract.SetInputConnection(imageMathematics.GetOutputPort())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(self.IsoSurfaceValue)
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())

        self.IsoSurfaceValue = 0.0

    def ThresholdInitialize(self):

        self.PrintLog('Threshold initialization.')

        if self.Interactive:
            queryString = "Please input lower threshold (\'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)

            queryString = "Please input upper threshold (\'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)

        scalarRange = self.Image.GetScalarRange()

        thresholdedImage = self.Image

        if self.LowerThreshold != None or self.UpperThreshold != None:
            threshold = vtk.vtkImageThreshold()
            threshold.SetInputData(self.Image)
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

        self.IsoSurfaceValue = 0.0

    def FastMarchingInitialize(self):

        self.PrintLog('Fast marching initialization.')

        sourceSeedIds = vtk.vtkIdList()
        targetSeedIds = vtk.vtkIdList()
        if self.Interactive:
            queryString = "Please input lower threshold (\'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)

            queryString = "Please input upper threshold (\'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)

            queryString = 'Please place source seeds'
            sourceSeeds = self.SeedInput(queryString,0)

            queryString = 'Please place target seeds'
            targetSeeds = self.SeedInput(queryString,0)

            for i in range(sourceSeeds.GetNumberOfPoints()):
                sourceSeedIds.InsertNextId(self.Image.FindPoint(sourceSeeds.GetPoint(i)))

            for i in range(targetSeeds.GetNumberOfPoints()):
                targetSeedIds.InsertNextId(self.Image.FindPoint(targetSeeds.GetPoint(i)))

        else:
            for i in range(len(self.SourcePoints)//3):
                sourceSeedIds.InsertNextId(self.Image.ComputePointId([self.SourcePoints[3*i+0],self.SourcePoints[3*i+1],self.SourcePoints[3*i+2]]))
            for i in range(len(self.TargetPoints)//3):
                targetSeedIds.InsertNextId(self.Image.ComputePointId([self.TargetPoints[3*i+0],self.TargetPoints[3*i+1],self.TargetPoints[3*i+2]]))

        scalarRange = self.Image.GetScalarRange()

        thresholdedImage = self.Image

        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInputData(self.Image)
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

        scale = 1.0
        if scalarRange[1]-scalarRange[0] > 0.0:
            scale = 1.0 / (scalarRange[1]-scalarRange[0])

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInputData(thresholdedImage)
        shiftScale.SetShift(-scalarRange[0])
        shiftScale.SetScale(scale)
        shiftScale.SetOutputScalarTypeToFloat()
        shiftScale.Update()

        speedImage = shiftScale.GetOutput()

        fastMarching = vtkvmtk.vtkvmtkFastMarchingUpwindGradientImageFilter()
        fastMarching.SetInputData(speedImage)
        fastMarching.SetSeeds(sourceSeedIds)
        fastMarching.GenerateGradientImageOff()
        fastMarching.SetTargetOffset(100.0)
        fastMarching.SetTargets(targetSeedIds)
        if targetSeedIds.GetNumberOfIds() > 0:
            fastMarching.SetTargetReachedModeToOneTarget()
        else:
            fastMarching.SetTargetReachedModeToNoTargets()
        fastMarching.Update()

        subtract = vtk.vtkImageMathematics()
        subtract.SetInputConnection(fastMarching.GetOutputPort())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(-fastMarching.GetTargetValue())
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())

        self.IsoSurfaceValue = 0.0

    def CollidingFrontsInitialize(self):

        self.PrintLog('Colliding fronts initialization.')

        seedIds1 = vtk.vtkIdList()
        seedIds2 = vtk.vtkIdList()

        if self.Interactive:
            queryString = "Please input lower threshold (\'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)

            queryString = "Please input upper threshold (\'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)

            queryString = 'Please place two seeds'
            seeds = self.SeedInput(queryString,2)

            seedIds1.InsertNextId(self.Image.FindPoint(seeds.GetPoint(0)))
            seedIds2.InsertNextId(self.Image.FindPoint(seeds.GetPoint(1)))

        else:
            seedIds1.InsertNextId(self.Image.ComputePointId([self.SourcePoints[0],self.SourcePoints[1],self.SourcePoints[2]]))
            seedIds2.InsertNextId(self.Image.ComputePointId([self.TargetPoints[0],self.TargetPoints[1],self.TargetPoints[2]]))

        scalarRange = self.Image.GetScalarRange()

        thresholdedImage = self.Image

        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInputData(self.Image)
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

        scale = 1.0
        if scalarRange[1]-scalarRange[0] > 0.0:
            scale = 1.0 / (scalarRange[1]-scalarRange[0])

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInputData(thresholdedImage)
        shiftScale.SetShift(-scalarRange[0])
        shiftScale.SetScale(scale)
        shiftScale.SetOutputScalarTypeToFloat()
        shiftScale.Update()

        speedImage = shiftScale.GetOutput()

        collidingFronts = vtkvmtk.vtkvmtkCollidingFrontsImageFilter()
        collidingFronts.SetInputData(speedImage)
        collidingFronts.SetSeeds1(seedIds1)
        collidingFronts.SetSeeds2(seedIds2)
        collidingFronts.ApplyConnectivityOn()
        collidingFronts.StopOnTargetsOn()
        collidingFronts.Update()

        subtract = vtk.vtkImageMathematics()
        subtract.SetInputConnection(collidingFronts.GetOutputPort())
        subtract.SetOperationToAddConstant()
        subtract.SetConstantC(-10.0 * collidingFronts.GetNegativeEpsilon())
        subtract.Update()

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(subtract.GetOutput())

        self.IsoSurfaceValue = 0.0

    def MultipleCollidingFrontsInitialize(self):

        self.PrintLog('Multiple colliding fronts initialization.')
        if self.Interactive:
            queryString = "Please input lower threshold (\'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)
            queryString = "Please input upper threshold (\'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)

        scalarRange = self.Image.GetScalarRange()
        thresholdedImage = self.Image

        if (self.LowerThreshold is not None) | (self.UpperThreshold is not None):
            threshold = vtk.vtkImageThreshold()
            threshold.SetInputData(self.Image)
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

        scale = 1.0
        if scalarRange[1]-scalarRange[0] > 0.0:
            scale = 1.0 / (scalarRange[1]-scalarRange[0])

        shiftScale = vtk.vtkImageShiftScale()
        shiftScale.SetInputData(thresholdedImage)
        shiftScale.SetShift(-scalarRange[0])
        shiftScale.SetScale(scale)
        shiftScale.SetOutputScalarTypeToFloat()
        shiftScale.Update()
        speedImage = shiftScale.GetOutput()

        seedIdsList = []
        if self.Interactive:
            seeds = None
            while not seeds:
                self.ImageSeeder.Seeds.AddObserver('ModifiedEvent', self._SeedPickCallback)
                queryString = 'Please place an even number of seeds'
                seeds = self.SeedInput(queryString,0)
            for pointIds in range(self.NumSeedsSelected//2):
                seedIds1 = vtk.vtkIdList()
                seedIds2 = vtk.vtkIdList()
                seedIds1.InsertNextId(self.Image.FindPoint(seeds.GetPoint(pointIds*2)))
                seedIds2.InsertNextId(self.Image.FindPoint(seeds.GetPoint((pointIds*2)+1)))
                seedIdsList.append([seedIds1, seedIds2])
        else:
            self.PrintError('Non Interactive use not permitted when using Multiple Colliding '\
                            'Fronts Initialization method.')

        lsList = []
        for seedIds1, seedIds2 in seedIdsList:
            collidingFronts = vtkvmtk.vtkvmtkCollidingFrontsImageFilter()
            collidingFronts.SetInputData(speedImage)
            collidingFronts.SetSeeds1(seedIds1)
            collidingFronts.SetSeeds2(seedIds2)              
            collidingFronts.ApplyConnectivityOn()
            collidingFronts.StopOnTargetsOn()
            collidingFronts.Update()

            subtract = vtk.vtkImageMathematics()
            subtract.SetInputConnection(collidingFronts.GetOutputPort())
            subtract.SetOperationToAddConstant()
            subtract.SetConstantC(-10.0 * collidingFronts.GetNegativeEpsilon())
            subtract.Update()

            singleLevelSets = vtk.vtkImageData()
            singleLevelSets.DeepCopy(subtract.GetOutput())
            lsList.append(singleLevelSets)

        for counter, levelSet in enumerate(lsList):
            if counter == 0:
                combined = vtk.vtkImageData()
                combined.DeepCopy(levelSet)
                continue
            else:
                combine = vtk.vtkImageMathematics()
                combine.SetOperationToAdd()
                combine.SetInput1Data(combined)
                combine.SetInput2Data(levelSet)
                combine.Update()
                combined = vtk.vtkImageData()
                combined.DeepCopy(combine.GetOutput())
                

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(combined)

        self.IsoSurfaceValue = 0.0 

    def SeedInitialize(self):

        self.PrintLog('Seed initialization.')

        seedIds = vtk.vtkIdList()
        if self.Interactive:
            queryString = 'Please place seeds'
            seeds = self.SeedInput(queryString,0)
            for i in range(seeds.GetNumberOfPoints()):
                seedIds.InsertNextId(self.Image.FindPoint(seeds.GetPoint(i)))
        else:
            for i in range(len(self.SourcePoints)//3):
                seedIds.InsertNextId(self.Image.ComputePointId([self.SourcePoints[3*i+0],self.SourcePoints[3*i+1],self.SourcePoints[3*i+2]]))
            for i in range(len(self.TargetPoints)//3):
                seedIds.InsertNextId(self.Image.ComputePointId([self.TargetPoints[3*i+0],self.TargetPoints[3*i+1],self.TargetPoints[3*i+2]]))

        self.InitialLevelSets = vtk.vtkImageData()
        self.InitialLevelSets.DeepCopy(self.Image)

        levelSetsInputScalars = self.InitialLevelSets.GetPointData().GetScalars()
        levelSetsInputScalars.FillComponent(0,1.0)

        dimensions = self.Image.GetDimensions()
        for i in range(seedIds.GetNumberOfIds()):
            levelSetsInputScalars.SetComponent(seedIds.GetId(i),0,-1.0)

        dilateErode = vtk.vtkImageDilateErode3D()
        dilateErode.SetInputData(self.InitialLevelSets)
        dilateErode.SetDilateValue(-1.0)
        dilateErode.SetErodeValue(1.0)
        dilateErode.SetKernelSize(3,3,3)
        dilateErode.Update()

        self.InitialLevelSets.DeepCopy(dilateErode.GetOutput())

        self.IsoSurfaceValue = 0.0

    def DisplayLevelSetSurface(self,levelSets):

        value = 0.0
        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInputData(levelSets)
        marchingCubes.SetValue(0,value)
        marchingCubes.Update()

        self.Surface = marchingCubes.GetOutput()

        self.OutputText('Displaying.\n')

        self.SurfaceViewer.Surface = marchingCubes.GetOutput()
        self.SurfaceViewer.Display = 0
        self.SurfaceViewer.Opacity = 0.5
        self.SurfaceViewer.BuildView()

    def InitializationTypeValidator(self,text):
        if text in ['0','1','2','3','4','5']:
            return 1
        return 0

    def YesNoValidator(self,text):
        if text in ['n','y']:
            return 1
        return 0

    def MergeLevelSets(self):

        if self.MergedInitialLevelSets == None:
            self.MergedInitialLevelSets = vtk.vtkImageData()
            self.MergedInitialLevelSets.DeepCopy(self.InitialLevelSets)
        else:
            minFilter = vtk.vtkImageMathematics()
            minFilter.SetOperationToMin()
            minFilter.SetInput1Data(self.MergedInitialLevelSets)
            minFilter.SetInput2Data(self.InitialLevelSets)
            minFilter.Update()
            self.MergedInitialLevelSets = minFilter.GetOutput()

    def Execute(self):
        from vmtk import vmtkscripts
        if self.Image == None:
            self.PrintError('Error: no Image.')

        cast = vtk.vtkImageCast()
        cast.SetInputData(self.Image)
        cast.SetOutputScalarTypeToFloat()
        cast.Update()
        self.Image = cast.GetOutput()

        if self.NegateImage:
            scalarRange = self.Image.GetScalarRange()
            negate = vtk.vtkImageMathematics()
            negate.SetInputData(self.Image)
            negate.SetOperationToMultiplyByK()
            negate.SetConstantK(-1.0)
            negate.Update()
            shiftScale = vtk.vtkImageShiftScale()
            shiftScale.SetInputConnection(negate.GetOutputPort())
            shiftScale.SetShift(scalarRange[1]+scalarRange[0])
            shiftScale.SetOutputScalarTypeToFloat()
            shiftScale.Update()
            self.Image = shiftScale.GetOutput()

        if self.Interactive:
            if not self.vmtkRenderer:
                self.vmtkRenderer = vmtkscripts.vmtkRenderer()
                self.vmtkRenderer.Initialize()
                self.OwnRenderer = 1

            self.vmtkRenderer.RegisterScript(self)

            if not self.ImageSeeder:
                self.ImageSeeder = vmtkscripts.vmtkImageSeeder()
                self.ImageSeeder.vmtkRenderer = self.vmtkRenderer
                self.ImageSeeder.Image = self.Image
                self.ImageSeeder.Display = 0
                self.ImageSeeder.Execute()
                ##self.ImageSeeder.Display = 1
                self.ImageSeeder.BuildView()

            if not self.SurfaceViewer:
                self.SurfaceViewer = vmtkscripts.vmtkSurfaceViewer()
                self.SurfaceViewer.vmtkRenderer = self.vmtkRenderer

            initializationMethods = {
                                '0': self.CollidingFrontsInitialize,
                                '1': self.MultipleCollidingFrontsInitialize,
                                '2': self.FastMarchingInitialize,
                                '3': self.ThresholdInitialize,
                                '4': self.IsosurfaceInitialize,
                                '5': self.SeedInitialize,
                                }

            endInitialization = False
            while not endInitialization:
                queryString = 'Please choose initialization type: \n'\
                              '0: colliding fronts;\n'\
                              '1: multiple colliding fronts;\n'\
                              '2: fast marching;\n'\
                              '3: threshold;\n'\
                              '4: isosurface;\n'\
                              '5: seed;\n'
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
            self.MergedInitialLevelSets = None

        else:
            if self.Method == "collidingfronts":
                self.CollidingFrontsInitialize()
            elif self.Method == "multiplecollidingfronts":
                self.MultipleCollidingFrontsInitialize()
            elif self.Method == "fastmarching":
                self.FastMarchingInitialize()
            elif self.Method == "threshold":
                self.ThresholdInitialize()
            elif self.Method == "isosurface":
                self.IsosurfaceInitialize()
            elif self.Method == "seeds":
                self.SeedInitialize()

        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
