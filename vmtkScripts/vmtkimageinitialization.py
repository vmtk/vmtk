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

        self.Interactive = 1

        self.vmtkRenderer = None
        self.OwnRenderer = 0

        self.Method = 'collidingfronts'
        self.IsosurfaceValue = 0.0
        self.UpperThreshold = 1E20
        self.LowerThreshold = -1E20
        self.Seeds = []
        self.TargetSeeds = []

        self.ImageSeeder = None

        self.SetScriptName('vmtkimageinitialization')
        self.SetInputMembers([
            ['Image','i','vtkImageData',1,'','vmtkimagereader'],
            ['Method','method','str',1,'the initialization method (collidingfronts, fastmarching, threshold, isosurface)'],
            ['Interactive','interactive','int',1],
            ['IsosurfaceValue','isosurfacevalue','float',1],
            ['UpperThreshold','upperthreshold','float',1],
            ['LowerThreshold','lowerthreshold','float',1],
            ['Seeds','seeds','float',-1],
            ['TargetSeeds','targetseeds','float',-1],
            ['vmtkRenderer','renderer','vmtkRenderer',1]
            ])
        self.SetOutputMembers([
            ['Image','o','vtkImageData',1,'','vmtkimagewriter']
            ])

    def SeedsToPoints(self,seeds):

        points = vtk.vtkPoints()
        if len(seeds) % 3 != 0:
            self.PrintError('Error: seeds not made up of triplets.')
            return points

        for i in range(len(seeds)/3):
            point = [seeds[3*i+0],seeds[3*i+1],seeds[3*i+2]]
            points.InsertNextPoint(point)

        return points

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

        if self.Interactive:
            queryString = "Please input isosurface level (\'i\' to activate image, \'n\' for none): "
            self.IsosurfaceValue = self.ThresholdInput(queryString)
        
        imageMathematics = vtk.vtkImageMathematics()
        imageMathematics.SetInput(self.Image)
        imageMathematics.SetConstantK(-1.0)
        imageMathematics.SetOperationToMultiplyByK()
        imageMathematics.Update()

        self.IsosurfaceValue *= -1.0

        self.Image = imageMathematics.GetOutput()

    def ThresholdInitialize(self):

        self.PrintLog('Threshold initialization.')

        if self.Interactive:
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
 
        self.Image = thresholdedImage
        self.IsosurfaceValue = 0.0

    def FastMarchingInitialize(self):

        self.PrintLog('Fast marching initialization.')

        if self.Interactive:
            queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)

            queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)

            queryString = 'Please place source seeds'
            sourceSeeds = self.SeedInput(queryString,0)
        
            queryString = 'Please place target seeds'
            targetSeeds = self.SeedInput(queryString,0)

        else:
            sourceSeeds = self.SeedsToPoints(self.Seeds)
            targetSeeds = self.SeedsToPoints(self.TargetSeeds)

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
        fastMarching.SetTargets(targetSeedIds)
        fastMarching.GenerateGradientImageOff()
        fastMarching.SetTargetOffset(100.0)
        fastMarching.SetTargetReachedModeToOneTarget()
##         fastMarching.SetTargetReachedModeToAllTargets()
        fastMarching.Update()

        self.Image = vtk.vtkImageData()
        self.Image.DeepCopy(collidingFronts.GetOutput())
        self.Image.Update()

        self.IsosurfaceValue = fastMarching.GetTargetValue()

    def CollidingFrontsInitialize(self):

        self.PrintLog('Colliding fronts initialization.')

        if self.Interactive:

            queryString = "Please input lower threshold (\'i\' to activate image, \'n\' for none): "
            self.LowerThreshold = self.ThresholdInput(queryString)
    
            queryString = "Please input upper threshold (\'i\' to activate image, \'n\' for none): "
            self.UpperThreshold = self.ThresholdInput(queryString)
    
            queryString = 'Please place two seeds'
            seeds = self.SeedInput(queryString,2)

            seedIds1 = vtk.vtkIdList()
            seedIds2 = vtk.vtkIdList()
        else:
            seeds = self.SeedsToPoints(self.Seeds)

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

        self.Image = vtk.vtkImageData()
        self.Image.DeepCopy(collidingFronts.GetOutput())
        self.Image.Update()

        self.IsosurfaceValue = 10.0 * collidingFronts.GetNegativeEpsilon()

    def Execute(self):
          
        if self.Image == None:
            self.PrintError('Error: no Image.')
        
        initializationMethods = {
                                'collidingfronts': self.CollidingFrontsInitialize,
                                'fastmarching': self.FastMarchingInitialize,
                                'threshold': self.ThresholdInitialize,
                                'isosurface': self.IsosurfaceInitialize
                                }

        if self.Interactive:

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
 
        initializationMethods[self.Method]()

        if self.IsosurfaceValue != 0:
            shiftScale = vtk.vtkImageShiftScale()
            shiftScale.SetInput(self.Image)
            shiftScale.SetShift(-self.IsosurfaceValue)
            shiftScale.Update()
            self.Image = shiftScale.GetOutput()
        
        if self.OwnRenderer:
            self.vmtkRenderer.Deallocate()
  

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
