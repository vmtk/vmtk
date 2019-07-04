#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceclipper.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:35:13 $
## Version:   $Revision: 1.9 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by 
##       Marco Fedele (marco.fedele@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes

class vmtkSurfaceThickening(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.CellEntityIdsArrayName = 'CellEntityIds'
        self.CellEntityIdsArray = None
        self.ThicknessArrayName = 'Thickness'
        self.ThicknessArray = None
        self.ThicknessThreshold = 0
        self.WarpFactor = 0.5
        self.WarpArrayName = 'WarpVector'
        self.CleanOutput = 1

        self.SetScriptName('vmtksurfacethickening')
        self.SetScriptDoc('warp a surface along its normal direction to obtain a surface thicker than a certain threshold')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['ThicknessArrayName','thicknessarray','str',1,'','name of the array where the thickness of the surface is defined'],
            ['ThicknessThreshold','threshold','float',1,'','all points on the surface where the thickness is lower than this threshold will be warped in normal direction to obtain a surface thick as this threshold'],
            ['WarpFactor','warpfactor','float',1,'','the surface is warped by [(thickness>threshold)*(threshold-thickness)*warpfactor+0]'],
            ['WarpArrayName','warparray','str',1,'','name of the array where the warping vector is stored']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])



    def Execute(self):
        from vmtk import vmtkscripts


        if self.Surface == None:
            self.PrintError('Error: no Surface.')

        self.ThicknessArray = self.Surface.GetPointData().GetArray(self.ThicknessArrayName)
        if self.ThicknessArray == None:
            self.PrintError('Error: no ThicknessArray called '+self.ThicknessArrayName+' defined on the surface')

        normalFilter = vmtkscripts.vmtkSurfaceNormals()
        normalFilter.Surface = self.Surface
        normalFilter.Execute()
        self.Surface = normalFilter.Surface

        calculator = vtk.vtkArrayCalculator()
        calculator.SetInputData(self.Surface)
        calculator.SetAttributeTypeToPointData()
        calculator.AddScalarVariable('thickness', self.ThicknessArrayName)
        calculator.AddVectorVariable('n', 'Normals')
        calculator.SetResultArrayName(self.WarpArrayName)
        calculator.SetFunction( str(self.WarpFactor) + ' * n * ( (thickness < ' + str(self.ThicknessThreshold) + ') * (' + str(self.ThicknessThreshold) + ' - thickness) + 0.0 )')

        self.PrintLog('calculator = '+calculator.GetFunction())
        calculator.Update()

        self.Surface = calculator.GetOutput()
        self.Surface.GetPointData().SetActiveVectors(self.WarpArrayName)

        warper = vtk.vtkWarpVector()
        warper.SetInputData(self.Surface)
        warper.SetScaleFactor(1.) # it is already in the calculator
        warper.Update()

        self.Surface = warper.GetOutput()




if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()


