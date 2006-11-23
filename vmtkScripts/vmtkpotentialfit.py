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
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 1.0
        self.Convergence = 1E-3
        self.MaxTimeStep = 1.0
        self.Relaxation = 1E-1
        self.Dimensionality = 3

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
            ['Dimensionality','dimensionality','int',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Image == None):
            self.PrintError('Error: Image not set.')
        
        gradientFilter = vtk.vtkImageGradient()
        gradientFilter.SetInput(self.Image)
        gradientFilter.SetDimensionality(self.Dimensionality)
        gradientFilter.Update()

        polyDataPotentialFit = vtkvmtk.vtkvmtkPolyDataPotentialFit()
        polyDataPotentialFit.SetInput(self.Surface)
        polyDataPotentialFit.SetPotentialImage(gradientFilter.GetOutput())
        polyDataPotentialFit.SetMaxTimeStep(self.MaxTimeStep)
        polyDataPotentialFit.SetRelaxation(self.Relaxation)
        polyDataPotentialFit.SetConvergence(self.Convergence)
        polyDataPotentialFit.SetPotentialWeight(self.PotentialWeight)
        polyDataPotentialFit.SetStiffnessWeight(self.StiffnessWeight)
        polyDataPotentialFit.SetNumberOfStiffnessSubIterations(self.NumberOfStiffnessSubIterations)
        polyDataPotentialFit.SetNumberOfIterations(self.NumberOfIterations)
        polyDataPotentialFit.Update()

        self.Surface = polyDataPotentialFit.GetOutput()

if __name__=='__main__':
    
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
