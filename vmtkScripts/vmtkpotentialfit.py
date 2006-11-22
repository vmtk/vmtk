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

import pypes 

vmtkpotentialfit = 'vmtkPotentialFit'

class vmtkPotentialFit(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
        
        self.Surface = None
        self.Image = None

        self.PotentialImage = None

        self.NumberOfIterations = 100
        self.NumberOfStiffnessSubIterations = 5
        self.Relaxation = 0.1
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 1.0
        self.Convergence = 1E-1
        self.MaxTimeStep = 1.0
        self.Relaxation = 1E-1

        self.InplanePotential = 0
        self.InplaneTolerance = 1E-4

        self.SetScriptName('vmtkpotentialfit')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','vmtksurfacereader'], 
            ['Image','image','vtkImageData',1,'','vmtkimagereader'],
            ['NumberOfIterations','iterations','int',1],
            ['NumberOfStiffnessSubIterations','subiterations','int',1],
            ['PotentialWeight','potentialweight','float',1],
            ['StiffnessWeight','stiffnessweight','float',1],
            ['Convergence','convergence','float',1],
            ['MaxTimeStep','maxtimestep','float',1],
            ['Relaxation','relaxation','float',1],
            ['InplanePotential','inplanepotential','int',1],
            ['InplaneTolerance','inplanetolerance','float',1] 
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','vmtksurfacewriter'],
            ['PotentialImage','potentialimage','vtkImageData',1,'','vmtkimagewriter']
            ])

    def SetRequiredImageType(self):
        cast = vtk.vtkImageCast()
        cast.SetInput(self.Image)
        cast.SetOutputScalarTypeToDouble()
        cast.Update()
        self.Image = cast.GetOutput()

    def SetImageRequirements(self):
        self.SetRequiredImageType()

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
    
    def Execute(self):

        if (self.Image == None):
            self.PrintError('Error: Image not set.')

        self.SetImageRequirements()

        self.BuildPotentialImage()

        polyDataPotentialFit = vtkvmtk.vtkvmtkPolyDataPotentialFit()
        polyDataPotentialFit.SetInput(self.Surface)
        polyDataPotentialFit.SetPotentialImage(self.PotentialImage)
        polyDataPotentialFit.SetMaxTimeStep(self.MaxTimeStep)
        polyDataPotentialFit.SetRelaxation(self.Relaxation)
        polyDataPotentialFit.SetConvergence(self.Convergence)
        polyDataPotentialFit.SetPotentialWeight(self.PotentialWeight)
        polyDataPotentialFit.SetStiffnessWeight(self.StiffnessWeight)
        polyDataPotentialFit.SetInplanePotential(self.InplanePotential)
        polyDataPotentialFit.SetInplaneTolerance(self.InplaneTolerance)
        polyDataPotentialFit.SetNumberOfStiffnessSubIterations(self.NumberOfStiffnessSubIterations)
        polyDataPotentialFit.SetNumberOfIterations(self.NumberOfIterations)
        polyDataPotentialFit.Update()

        self.Surface = polyDataPotentialFit.GetOutput()

if __name__=='__main__':
    
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
