#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbranchgeometry.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.7 $

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


class vmtkBranchGeometry(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.GeometryData = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.GroupIdsArrayName = 'GroupIds'
        self.BlankingArrayName = 'Blanking'

        self.LengthArrayName = 'Length'
        self.CurvatureArrayName = 'Curvature'
        self.TorsionArrayName = 'Torsion'
        self.TortuosityArrayName = 'Tortuosity'

        self.LineSmoothing = 0
        self.NumberOfSmoothingIterations = 100
        self.SmoothingFactor = 0.1

        self.SetScriptName('vmtkbranchgeometry')
        self.SetScriptDoc('compute geometric parameters for each branch of a tree. The script takes in input the centerlines already split into branches.')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input split centerlines','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius values are stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','name of the array where blanking information about branches is stored'],
            ['LengthArrayName','lengtharray','str',1,'','name of the array where the average length of each branch has to be stored'],
            ['CurvatureArrayName','curvaturearray','str',1,'','name of the array where the average curvature of each branch has to be stored'],
            ['TorsionArrayName','torsionarray','str',1,'','name of the array where the average torsion of each branch has to be stored'],
            ['TortuosityArrayName','tortuosityarray','str',1,'','name of the array where the average tortuosity of each branch, defined as the length of a line divided by the distance of its endpoints, has to be stored'],
            ['LineSmoothing','smoothing','bool',1,''],
            ['NumberOfSmoothingIterations','iterations','int',1,'(0,)'],
            ['SmoothingFactor','factor','float',1,'(0.0,)']
            ])
        self.SetOutputMembers([
            ['GeometryData','o','vtkPolyData',1,'','the output data set','vmtksurfacewriter'],
            ['LengthArrayName','lengtharray','str',1,'','name of the array where the average length of each branch is stored'],
            ['CurvatureArrayName','curvaturearray','str',1,'','name of the array where the average curvature of each branch is stored'],
            ['TorsionArrayName','torsionarray','str',1,'','name of the array where the average torsion of each branch is stored'],
            ['TortuosityArrayName','tortuosityarray','str',1,'','name of the array where the average tortuosity of each branch, defined as the length of a line divided by the distance of its endpoints minus one (L/D - 1), is stored']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        centerlineBranchGeometry = vtkvmtk.vtkvmtkCenterlineBranchGeometry()
        centerlineBranchGeometry.SetInputData(self.Centerlines)
        centerlineBranchGeometry.SetRadiusArrayName(self.RadiusArrayName)
        centerlineBranchGeometry.SetGroupIdsArrayName(self.GroupIdsArrayName)
        centerlineBranchGeometry.SetBlankingArrayName(self.BlankingArrayName)
        centerlineBranchGeometry.SetLengthArrayName(self.LengthArrayName)
        centerlineBranchGeometry.SetCurvatureArrayName(self.CurvatureArrayName)
        centerlineBranchGeometry.SetTorsionArrayName(self.TorsionArrayName)
        centerlineBranchGeometry.SetTortuosityArrayName(self.TortuosityArrayName)
        centerlineBranchGeometry.SetLineSmoothing(self.LineSmoothing)
        centerlineBranchGeometry.SetNumberOfSmoothingIterations(self.NumberOfSmoothingIterations)
        centerlineBranchGeometry.SetSmoothingFactor(self.SmoothingFactor)

        centerlineBranchGeometry.Update()

        self.GeometryData = centerlineBranchGeometry.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
