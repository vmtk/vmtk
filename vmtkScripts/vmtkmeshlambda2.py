#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshlambda2.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
from vmtk import vtkvmtk
import vtk
import sys

from vmtk import pypes


class vmtkMeshLambda2(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.VelocityArrayName = None
        self.Lambda2ArrayName = 'Lambda2'

        self.ConvergenceTolerance = 1E-6
        self.QuadratureOrder = 3

        self.SetScriptName('vmtkmeshlambda2')
        self.SetScriptDoc('compute lambda2 from a velocity field')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['VelocityArrayName','velocityarray','str',1,'',''],
            ['Lambda2ArrayName','lambda2array','str',1,'',''],
            ['ConvergenceTolerance','tolerance','float',1,'',''],
            ['QuadratureOrder','quadratureorder','int',1,'','']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        lambda2Filter = vtkvmtk.vtkvmtkMeshLambda2()
        lambda2Filter.SetInputData(self.Mesh)
        lambda2Filter.SetVelocityArrayName(self.VelocityArrayName)
        lambda2Filter.SetLambda2ArrayName(self.Lambda2ArrayName)
        lambda2Filter.SetConvergenceTolerance(self.ConvergenceTolerance)
        lambda2Filter.SetQuadratureOrder(self.QuadratureOrder)
        lambda2Filter.ComputeIndividualPartialDerivativesOn()
        lambda2Filter.ForceBoundaryToNegativeOn()
        lambda2Filter.Update()

        self.Mesh = lambda2Filter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
