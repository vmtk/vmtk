#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkmeshpolyballevaluation.py,v $
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


class vmtkMeshPolyBallEvaluation(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Mesh = None
        self.PolyBall = None
        self.RadiusArrayName = None
        self.EvaluationArrayName = 'PolyBall'
        self.Type = "polyball"

        self.SetScriptName('vmtkmeshpolyballevaluation')
        self.SetScriptDoc('evaluate the polyball function on the vertices of a mesh.')
        self.SetInputMembers([
            ['Mesh','i','vtkUnstructuredGrid',1,'','the input mesh','vmtkmeshreader'],
            ['PolyBall','polyball','vtkPolyData',1,'','the input polyball','vmtksurfacereader'],
            ['Type','type','str',1,'["polyball","tubes"]','type of evaluation, polyball (set of disjoint spheres) or tubes (set of continuous tubes, e.g. centerlines)'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where the radius of polyballs is stored'],
            ['EvaluationArrayName','evaluationarray','str',1,'','name of the array where the result of the polyball evaluation has to be stored']
            ])
        self.SetOutputMembers([
            ['Mesh','o','vtkUnstructuredGrid',1,'','the output mesh','vmtkmeshwriter']
            ])

    def Execute(self):

        if self.Mesh == None:
            self.PrintError('Error: No input mesh.')

        if self.PolyBall == None:
            self.PrintError('Error: No input polyball.')

        evaluationArray = vtk.vtkDoubleArray()
        evaluationArray.SetName(self.EvaluationArrayName)
        evaluationArray.SetNumberOfComponents(1)
        evaluationArray.SetNumberOfTuples(self.Mesh.GetNumberOfPoints())

        polyball = None
        if self.Type == "polyball":
            polyball = vtkvmtk.vtkvmtkPolyBall()
        elif self.Type == "tubes":
            polyball = vtkvmtk.vtkvmtkPolyBallLine()
        polyball.SetInputData(self.PolyBall)
        polyball.SetPolyBallRadiusArrayName(self.RadiusArrayName)

        for i in range(self.Mesh.GetNumberOfPoints()):
            point = self.Mesh.GetPoint(i)
            value = polyball.EvaluateFunction(point)
            evaluationArray.SetValue(i,value)

        self.Mesh.GetPointData().AddArray(evaluationArray)


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
