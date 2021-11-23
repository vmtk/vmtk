#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkpotentialfit.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:51:28 $
## Version:   $Revision: 1.5 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
from vmtk import vtkvmtk
import sys

from vmtk import pypes


class vmtkPotentialFit(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Image = None

        self.PotentialImage = None

        self.NumberOfIterations = 100
        self.NumberOfStiffnessSubIterations = 0
        self.NumberOfInflationSubIterations = 0
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 1.0
        self.InflationWeight = 1.0
        self.Convergence = 1E-5
        self.MaxTimeStep = 1.0
        self.TimeStep = 1.0
        self.AdaptiveTimeStep = 1
        self.Relaxation = 1E-1
        self.Dimensionality = 3

        self.SetScriptName('vmtkpotentialfit')
        self.SetScriptDoc('explicitly deformable model which evolves a surface to gradient magnitudes of an input image')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Image','image','vtkImageData',1,'','','vmtkimagereader'],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['NumberOfStiffnessSubIterations','stiffnesssubiterations','int',1,'(0,)'],
            ['NumberOfInflationSubIterations','inflationsubiterations','int',1,'(0,)'],
            ['PotentialWeight','potentialweight','float',1,'(0.0,)'],
            ['StiffnessWeight','stiffnessweight','float',1,'(0.0,)'],
            ['InflationWeight','inflationweight','float',1,'(0.0,)'],
            ['Convergence','convergence','float',1,'(0.0,)'],
            ['MaxTimeStep','maxtimestep','float',1,'(0.0,)'],
            ['TimeStep','timestep','float',1,'(0.0,)'],
            ['AdaptiveTimeStep','adaptivetimestep','bool',1],
            ['Relaxation','relaxation','float',1,'(0.0,)'],
            ['Dimensionality','dimensionality','int',1,'(2,3)']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Image == None):
            self.PrintError('Error: Image not set.')

        polyDataPotentialFit = vtkvmtk.vtkvmtkPolyDataPotentialFit()
        polyDataPotentialFit.SetInputData(self.Surface)
        polyDataPotentialFit.SetPotentialImage(self.Image)
        polyDataPotentialFit.SetMaxTimeStep(self.MaxTimeStep)
        polyDataPotentialFit.SetTimeStep(self.TimeStep)
        polyDataPotentialFit.SetAdaptiveTimeStep(self.AdaptiveTimeStep)
        polyDataPotentialFit.SetRelaxation(self.Relaxation)
        polyDataPotentialFit.SetConvergence(self.Convergence)
        polyDataPotentialFit.SetPotentialWeight(self.PotentialWeight)
        polyDataPotentialFit.SetStiffnessWeight(self.StiffnessWeight)
        polyDataPotentialFit.SetInflationWeight(self.InflationWeight)
        polyDataPotentialFit.SetNumberOfStiffnessSubIterations(self.NumberOfStiffnessSubIterations)
        polyDataPotentialFit.SetNumberOfInflationSubIterations(self.NumberOfInflationSubIterations)
        polyDataPotentialFit.SetNumberOfIterations(self.NumberOfIterations)
        polyDataPotentialFit.SetDimensionality(self.Dimensionality)
        polyDataPotentialFit.Update()

        self.Surface = polyDataPotentialFit.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
