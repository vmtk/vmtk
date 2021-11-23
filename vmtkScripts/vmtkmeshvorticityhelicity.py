#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshvorticityhelicity.py,v $
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


class vmtkMeshVorticityHelicity(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None

        self.VelocityArrayName = None
        self.VorticityArrayName = 'Vorticity'
        self.HelicityArrayName = 'Helicity'

        self.ComputeHelicity = True
        self.ConvergenceTolerance = 1E-6
        self.QuadratureOrder = 3

        self.SetScriptName('vmtkmeshvorticityhelicity')
        self.SetScriptDoc('compute vorticity and helicity from a velocity field')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['ComputeHelicity','helicity','bool',1,'',''],
            ['VelocityArrayName','velocityarray','str',1,'',''],
            ['VorticityArrayName','vorticityarray','str',1,'',''],
            ['HelicityArrayName','helicityarray','str',1,'',''],
            ['ConvergenceTolerance','tolerance','float',1,'',''],
            ['QuadratureOrder','quadratureorder','int',1,'','']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if (self.Mesh == None):
            self.PrintError('Error: no Mesh.')

        vorticityFilter = vtkvmtk.vtkvmtkUnstructuredGridVorticityFilter()
        vorticityFilter.SetInputData(self.Mesh)
        vorticityFilter.SetVelocityArrayName(self.VelocityArrayName)
        vorticityFilter.SetVorticityArrayName(self.VorticityArrayName)
        vorticityFilter.SetHelicityFactorArrayName(self.HelicityArrayName)
        vorticityFilter.SetConvergenceTolerance(self.ConvergenceTolerance)
        vorticityFilter.SetQuadratureOrder(self.QuadratureOrder)
        if self.ComputeHelicity:
            vorticityFilter.ComputeHelicityFactorOn()
        else:
            vorticityFilter.ComputeHelicityFactorOff()
        vorticityFilter.Update()

        self.Mesh = vorticityFilter.GetOutput()


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
