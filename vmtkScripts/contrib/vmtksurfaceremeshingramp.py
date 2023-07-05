#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfaceremeshingramp.py,v $
## Language:  Python
## Date:      $Date: 2021/06/10 10:00:00 $
## Version:   $Revision: 1.0 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Michele Bucelli (michele.bucelli@polimi.it)
##       Politecnico di Milano

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
from vmtk import pypes

vmtksurfaceremeshingramp = 'vmtkSurfaceRemeshingRamp'

class vmtkSurfaceRemeshingRamp(pypes.pypeScript):

    def __init__(self):
        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.EdgeLengthInitial = 1.0
        self.EdgeLengthFinal = 5.0
        self.RampSteps = 2
        self.RemeshingIterations = 10
        self.CellEntityIdsArrayName = 'CellEntityIds'

        self.SetScriptName('vmtksurfaceremeshingramp')
        self.SetScriptDoc('remesh a surface performing several vmtksurfaceremeshing' \
            'steps, using edge length as a remeshing parameter, varying the target' \
            'edge length with a linear ramp from an initial to a final value')
        self.SetInputMembers([
            ['Surface', 'i', 'vtkPolyData', 1, '', 'the input surface', 'vmtksurfacereader'],
            ['EdgeLengthInitial', 'edgelengthinitial', 'float', 1, '(0.0,)'],
            ['EdgeLengthFinal', 'edgelengthfinal', 'float', 1, '(0.0,)'],
            ['RampSteps', 'rampsteps', 'int', 1, '(1,)'],
            ['CellEntityIdsArrayName', 'entityidsarray', 'str', 1],
            ['RemeshingIterations', 'iterations', 'int', 1, '(1,)']
            ])
        self.SetOutputMembers([
            ['Surface', 'o', 'vtkPolyData', 1, '', 'the output surface', 'vmtksurfacewriter']
            ])

    def Execute(self):
        from vmtk import vmtkscripts

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        for i in range(self.RampSteps+1):
            edgelength = self.EdgeLengthInitial + i * (self.EdgeLengthFinal - self.EdgeLengthInitial) / self.RampSteps
            self.PrintLog('Remeshing with edge length = ' + str(edgelength))
            sr = vmtkscripts.vmtkSurfaceRemeshing()
            sr.Surface = self.Surface
            sr.ElementSizeMode = 'edgelength'
            sr.TargetEdgeLength = edgelength
            sr.CellEntityIdsArrayName = self.CellEntityIdsArrayName
            sr.NumberOfIterations = self.RemeshingIterations
            sr.CleanOutput = 1
            sr.Execute()
            self.Surface = sr.Surface

if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
