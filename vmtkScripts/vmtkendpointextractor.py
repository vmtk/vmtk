#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkendpointextractor.py,v $
## Language:  Python
## Date:      $Date: 2006/03/01 11:54:16 $
## Version:   $Revision: 1.9 $

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


class vmtkEndpointExtractor(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'

        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.BlankingArrayName = 'Blanking'
        self.TractIdsArrayName = 'TractIds'

        self.NumberOfEndpointSpheres = 2
        self.NumberOfGapSpheres = 1

        self.SetScriptName('vmtkendpointextractor')
        self.SetScriptDoc('find the endpoints of a split and grouped centerline')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['TractIdsArrayName','tractidsarray','str',1],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['RadiusArrayName','radiusarray','str',1],
            ['BlankingArrayName','blankingarray','str',1],
            ['NumberOfEndpointSpheres','numberofendpointspheres','int',1,'(0.0,)'],
            ['NumberOfGapSpheres','numberofgapspheres','int',1,'(0.0,)']
            ])
        self.SetOutputMembers([
            ['Centerlines','o','vtkPolyData',1,'','','vmtksurfacewriter'],
            ['GroupIdsArrayName','groupidsarray','str',1],
            ['TractIdsArrayName','tractidsarray','str',1],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1],
            ['BlankingArrayName','blankingarray','str',1]
            ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        endpointExtractor = vtkvmtk.vtkvmtkCenterlineEndpointExtractor()
        endpointExtractor.SetInputData(self.Centerlines)
        endpointExtractor.SetRadiusArrayName(self.RadiusArrayName)
        endpointExtractor.SetGroupIdsArrayName(self.GroupIdsArrayName)
        endpointExtractor.SetTractIdsArrayName(self.TractIdsArrayName)
        endpointExtractor.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        endpointExtractor.SetBlankingArrayName(self.BlankingArrayName)
        endpointExtractor.SetNumberOfEndpointSpheres(self.NumberOfEndpointSpheres)
        endpointExtractor.SetNumberOfGapSpheres(self.NumberOfGapSpheres)
        endpointExtractor.Update()

        self.Centerlines = endpointExtractor.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
