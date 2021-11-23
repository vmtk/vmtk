#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkactivetubes.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.3 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

##   Developed with support from the EC FP7/2007-2013: ARCH, Project n. 224390

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes


class vmtkActiveTubes(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerline = None
        self.Image = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'

        self.NumberOfIterations = 100
        self.PotentialWeight = 1.0
        self.StiffnessWeight = 1.0
        self.Convergence = 1E-5
        self.CFLCoefficient = 0.1
        self.MinimumRadius = 0.0
        self.NumberOfAngularEvaluations = 16
        self.NegativeNormWarnings = False

        self.SetScriptName('vmtkactivetubes')
        self.SetScriptDoc('experimental method for generating centerlines from an image.')
        self.SetInputMembers([
            ['Centerline','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Image','image','vtkImageData',1,'','','vmtkimagereader'],
            ['RadiusArrayName','radiusarray','str',1,''],
            ['NumberOfIterations','iterations','int',1,'(0,)'],
            ['PotentialWeight','potentialweight','float',1,'(0.0,)'],
            ['StiffnessWeight','stiffnessweight','float',1,'(0.0,)'],
            ['Convergence','convergence','float',1,'(0.0,)'],
            ['CFLCoefficient','cfl','float',1,'(0.0,)'],
            ['MinimumRadius','minradius','float',1,'(0.0,)'],
            ['NegativeNormWarnings','warnings','bool',1,''],
            ['NumberOfAngularEvaluations','angularevaluations','int',1,'(0,)']]
            )
        self.SetOutputMembers([
            ['Centerline','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Image == None:
            self.PrintError('Error: Image not set.')

        activeTubes = vtkvmtk.vtkvmtkActiveTubeFilter()
        activeTubes.SetInputData(self.Centerline)
        activeTubes.SetPotentialImage(self.Image)
        activeTubes.SetRadiusArrayName(self.RadiusArrayName)
        activeTubes.SetConvergence(self.Convergence)
        activeTubes.SetPotentialWeight(self.PotentialWeight)
        activeTubes.SetStiffnessWeight(self.StiffnessWeight)
        activeTubes.SetNumberOfIterations(self.NumberOfIterations)
        activeTubes.SetCFLCoefficient(self.CFLCoefficient)
        activeTubes.SetMinimumRadius(self.MinimumRadius)
        activeTubes.SetNumberOfAngularEvaluations(self.NumberOfAngularEvaluations)
        activeTubes.SetNegativeNormWarnings(self.NegativeNormWarnings)
        activeTubes.Update()

        self.Centerline = activeTubes.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
