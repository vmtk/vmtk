#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbranchextractor.py,v $
## Language:  Python
## Date:      $Date: 2006/02/23 09:31:39 $
## Version:   $Revision: 1.11 $

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


class vmtkBranchExtractor(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'

        self.BlankingArrayName = 'Blanking'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'

        self.SetScriptName('vmtkbranchextractor')
        self.SetScriptDoc('split and group centerlines along branches')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['TractIdsArrayName','tractidsarray','str',1],
                    ['BlankingArrayName','blankingarray','str',1],
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['TractIdsArrayName','tractidsarray','str',1],
            ['BlankingArrayName','blankingarray','str',1]
            ])

    def Execute(self):

        if not self.Centerlines:
            self.PrintError('Error: No input centerlines.')

        branchExtractor = vtkvmtk.vtkvmtkCenterlineBranchExtractor()
        branchExtractor.SetInputData(self.Centerlines)
        branchExtractor.SetBlankingArrayName(self.BlankingArrayName)
        branchExtractor.SetRadiusArrayName(self.RadiusArrayName)
        branchExtractor.SetGroupIdsArrayName(self.GroupIdsArrayName)
        branchExtractor.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        branchExtractor.SetTractIdsArrayName(self.TractIdsArrayName)
        branchExtractor.Update()

        self.Centerlines = branchExtractor.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
