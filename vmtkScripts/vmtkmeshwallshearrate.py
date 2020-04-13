#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshwallshearrate.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: full tensor WSR computation was contributed by
##       Ulf Shiller & Mehrdad Yousefi, Clemson University
##       Mehdi Najafi, University of Toronto

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
from vmtk import vtkvmtk
import vtk
import sys

from vmtk import pypes


class vmtkMeshWallShearRate(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.Surface = None

        self.VelocityArrayName = None
        self.WallShearRateArrayName = 'WallShearRate'

        self.ConvergenceTolerance = 1E-6
        self.QuadratureOrder = 3
        self.UseFullStrainRateTensor = 0

        self.SetScriptName('vmtkmeshwallshearrate')
        self.SetScriptDoc('compute wall shear rate from a velocity field, producing a surface in output')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['VelocityArrayName','velocityarray','str',1,'',''],
            ['WallShearRateArrayName','wsrarray','str',1,'',''],
            ['ConvergenceTolerance','tolerance','float',1,'',''],
            ['UseFullStrainRateTensor','fulltensor','bool',1,'',''],
            ['QuadratureOrder','quadratureorder','int',1,'','']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        wallShearRateFilter = vtkvmtk.vtkvmtkMeshWallShearRate()
        wallShearRateFilter.SetInputData(self.Mesh)
        wallShearRateFilter.SetVelocityArrayName(self.VelocityArrayName)
        wallShearRateFilter.SetWallShearRateArrayName(self.WallShearRateArrayName)
        wallShearRateFilter.SetConvergenceTolerance(self.ConvergenceTolerance)
        wallShearRateFilter.SetQuadratureOrder(self.QuadratureOrder)
        wallShearRateFilter.SetUseFullStrainRateTensor(self.UseFullStrainRateTensor)
        wallShearRateFilter.ComputeIndividualPartialDerivativesOn()
        wallShearRateFilter.Update()

        self.Surface = wallShearRateFilter.GetOutput()



if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
