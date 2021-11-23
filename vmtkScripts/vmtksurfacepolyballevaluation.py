#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacepolyballevaluation.py,v $
## Language:  Python
## Date:      $Date: 2006/07/17 09:53:14 $
## Version:   $Revision: 1.8 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import vtk
from vmtk import vtkvmtk

from vmtk import pypes


class vmtkSurfacePolyBallEvaluation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.PolyBall = None
        self.RadiusArrayName = None
        self.EvaluationArrayName = 'PolyBall'
        self.Type = "polyball"

        self.SetScriptName('vmtksurfacepolyballevaluation')
        self.SetScriptDoc('evaluate the polyball function on the vertices of a surface.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['PolyBall','polyball','vtkPolyData',1,'','the input polyball','vmtksurfacereader'],
            ['Type','type','str',1,'["polyball","tubes"]','type of evaluation, polyball (set of disjoint spheres) or tubes (set of continuous tubes, e.g. centerlines)'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where the radius of polyballs is stored'],
            ['EvaluationArrayName','evaluationarray','str',1,'','name of the array where the result of the polyball evaluation has to be stored']
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','the output surface','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.PolyBall == None:
            self.PrintError('Error: No input polyball.')

        evaluationArray = vtk.vtkDoubleArray()
        evaluationArray.SetName(self.EvaluationArrayName)
        evaluationArray.SetNumberOfComponents(1)
        evaluationArray.SetNumberOfTuples(self.Surface.GetNumberOfPoints())

        polyball = None
        if self.Type == "polyball":
            polyball = vtkvmtk.vtkvmtkPolyBall()
        elif self.Type == "tubes":
            polyball = vtkvmtk.vtkvmtkPolyBallLine()
        polyball.SetInputData(self.PolyBall)
        polyball.SetPolyBallRadiusArrayName(self.RadiusArrayName)

        for i in range(self.Surface.GetNumberOfPoints()):
            point = self.Surface.GetPoint(i)
            value = polyball.EvaluateFunction(point)
            evaluationArray.SetValue(i,value)

        self.Surface.GetPointData().AddArray(evaluationArray)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
