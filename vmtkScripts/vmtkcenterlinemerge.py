#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkcenterlinemerge.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:49:59 $
## Version:   $Revision: 1.4 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes
from vmtk import vtkvmtk


class vmtkCenterlineMerge(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'
        self.BlankingArrayName = 'Blanking'

        self.Length = 0.0
        self.MergeBlanked = 1

        self.SetScriptName('vmtkcenterlinemerge')
        self.SetScriptDoc('merge centerline tracts belonging to the same groups')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input centerlines','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius is stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1,'','name of the array where centerline ids are stored'],
            ['TractIdsArrayName','tractidsarray','str',1,'','name of the array where centerline tract ids are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','name of the array where centerline blanking information about branches is stored'],
            ['Length','length','float',1,'(0.0,)','length of the resampling interval'],
            ['MergeBlanked','mergeblanked','bool',1,'','toggle generation of segments for blanked groups']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','the output centerlines','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        mergeCenterlines = vtkvmtk.vtkvmtkMergeCenterlines()
        mergeCenterlines.SetInputData(self.Centerlines)
        mergeCenterlines.SetRadiusArrayName(self.RadiusArrayName)
        mergeCenterlines.SetGroupIdsArrayName(self.GroupIdsArrayName)
        mergeCenterlines.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        mergeCenterlines.SetTractIdsArrayName(self.TractIdsArrayName)
        mergeCenterlines.SetBlankingArrayName(self.BlankingArrayName)
        mergeCenterlines.SetResamplingStepLength(self.Length)
        mergeCenterlines.SetMergeBlanked(self.MergeBlanked)
        mergeCenterlines.Update()

        self.Centerlines = mergeCenterlines.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
