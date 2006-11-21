#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkpotentialfit.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:51:28 $
## Version:   $Revision: 1.5 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

import vtk
import vtkvmtk
import sys
import string

import pypes 

vmtkpotentialfit = 'vmtkPotentialFit'

class vmtkPotentialFit(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Image = None
        self.ImageViewer = None
        self.SurfaceViewer = None
        self.Interactive = 1

        self.PotentialImage = None

        self.DisplayedImage = None

        self.Threshold = 0.0
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 1.0
        self.Relaxation = 1E-1
        self.Convergence = 1E-1        

        self.InplanePotential = 0
        self.InplaneTolerance = 1E-4

        self.Surface = 0

        self.SetScriptName('vmtkpotentialfit')
        self.SetInputMembers([
            ['Image','image','vtkImageData',1], 
            ['Interactive','interactive','int',1], 
            ['ImageViewer','imageviewer','vmtkImageViewer',1],['SurfaceViewer','surfaceviewer','vmtkSurfaceViewer',1], 
            ['Threshold','threshold','float',1],['Relaxation','relaxation','float',1],['Convergence','convergence','float',1], 
            ['InplanePotential','inplanepotential','int',1],['InplaneTolerance','inplaneTolerance','float',1], 
            ['PotentialWeight','potentialweight','float',1],['StiffnessWeight','stiffnessweight','float',1],['PotentialImage','potentialimage','vtkImageData',1]])
        self.SetOutputMembers([['Surface','o','vtkPolyData',1]])

    def SetImageType(self,image):
        if (image.GetScalarType != 'double'):
            cast = vtk.vtkImageCast()
            cast.SetInput(image)
            cast.SetOutputScalarTypeToDouble()
            cast.Update()
            image = cast.GetOutput()
            if image.GetSource():
                image.GetSource().UnRegisterAllOutputs()
        return image

    def SetImageRequirements(self,image):
        image = self.SetImageType(image)
        return image

    def BuildPotentialImage(self):
        gradientMagnitudeFilter = vtk.vtkImageGradientMagnitude()
        gradientMagnitudeFilter.SetInput(self.Image)
        gradientMagnitudeFilter.SetDimensionality(3)
        gradientMagnitudeFilter.Update()
        gradientFilter = vtk.vtkImageGradient()
        gradientFilter.SetInput(gradientMagnitudeFilter.GetOutput())
        gradientFilter.SetDimensionality(3)
        gradientFilter.Update()
        self.PotentialImage = gradientFilter.GetOutput()

        self.DisplayedImage = gradientMagnitudeFilter.GetOutput()
        if self.DisplayedImage.GetSource():
            self.DisplayedImage.GetSource().UnRegisterAllOutputs()
        if self.PotentialImage.GetSource():
            self.PotentialImage.GetSource().UnRegisterAllOutputs()
        self.PotentialImage.GetPointData().GetScalars().FillComponent(2,0.0) ######################### inplane force only
    
    def BuildInitialSurface(self, level):

        marchingExtent = self.Image.GetWholeExtent()
        translateExtent = vtk.vtkImageTranslateExtent()
        translateExtent.SetInput(self.Image)
        translateExtent.SetTranslation(-marchingExtent[0],-marchingExtent[2],-marchingExtent[4])
        translateExtent.Update()
        marchingCubes = vtk.vtkMarchingCubes()
        marchingCubes.SetInput(translateExtent.GetOutput())
        marchingCubes.SetValue(0,level)
        marchingCubes.Update()
        surface = marchingCubes.GetOutput()
        if surface.GetSource():
            surface.GetSource().UnRegisterAllOutputs()
        return surface

    def Execute(self):

        if (self.Image == None):
            self.PrintError('Error: Image not set.')
            sys.exit()

        self.Image = self.SetImageRequirements(self.Image)

        self.BuildPotentialImage()

        if self.Interactive == 1:
            print "Please probe image for initial threshold."
            if self.ImageViewer != None:
                self.ImageViewer.Image = self.Image
                self.ImageViewer.Display = 1
                self.ImageViewer.BuildView()

            isOk = 0
            while (isOk == 0):
                thresholdString = raw_input("Please input initial threshold (\'e\' to end, \'q\' to quit): ")
                if (thresholdString == ''):
                    continue
                if (thresholdString == 'e'):
                    isOk = 1
                    break
                elif (thresholdString == 'q'):
                    sys.exit()
                else:
                    invalid = 0
                    for char in thresholdString:
                        if char not in string.digits + '.' + ' ':
                            print 'Invalid input.'
                            invalid = 1
                            break
                    if invalid:
                        continue

                threshold = double(thresholdString)

                self.Surface = self.BuildInitialSurface(threshold)
                
                if self.SurfaceViewer != None:
                    self.SurfaceViewer.Surface = self.Surface
                    self.SurfaceViewer.Display = 1
                    self.SurfaceViewer.BuildView()
        else:
            self.Surface = self.BuildInitialSurface(self.Threshold)

        if self.ImageViewer != None:
            self.ImageViewer.Image = self.DisplayedImage
            self.ImageViewer.Display = 0
            self.ImageViewer.BuildView()

        polyDataPotentialFit = vtkvmtk.vtkvmtkPolyDataPotentialFit()
        polyDataPotentialFit.SetInput(self.Surface)
        polyDataPotentialFit.SetPotentialImage(self.PotentialImage)
        polyDataPotentialFit.SetRelaxation(self.Relaxation)
        polyDataPotentialFit.SetConvergence(self.Convergence)
        polyDataPotentialFit.SetPotentialWeight(self.PotentialWeight)
        polyDataPotentialFit.SetStiffnessWeight(self.StiffnessWeight)
        polyDataPotentialFit.SetInplanePotential(self.InplanePotential)
        polyDataPotentialFit.SetInplaneTolerance(self.InplaneTolerance)
        polyDataPotentialFit.SetNumberOfStiffnessSubIterations(10)
        polyDataPotentialFit.Update()

        self.Surface = polyDataPotentialFit.GetOutput()
        if self.Surface.GetSource():
            self.Surface.GetSource().UnRegisterAllOutputs()

        if self.SurfaceViewer != None:
            self.SurfaceViewer.Surface = self.Surface
            self.SurfaceViewer.Display = 1
            self.SurfaceViewer.BuildView()
        elif self.ImageViewer != None:
            self.ImageViewer.Image = self.DisplayedImage
            self.ImageViewer.Display = 1
            self.ImageViewer.BuildView()

if __name__=='__main__':
    
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
