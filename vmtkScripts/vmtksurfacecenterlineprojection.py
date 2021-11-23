#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtksurfacecenterlineprojection.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.5 $

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


class vmtkSurfaceCenterlineProjection(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.UseRadiusInformation = 0
        self.RadiusArrayName = 'MaximumInscribedSphereRadius'

        self.SetScriptName('vmtksurfacecenterlineprojection')
        self.SetScriptDoc('project centerline data onto surface points')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','','vmtksurfacereader'],
            ['UseRadiusInformation','useradius','bool',1],
            ['RadiusArrayName','radiusarray','str',1]
            ])
        self.SetOutputMembers([
            ['Surface','o','vtkPolyData',1,'','','vmtksurfacewriter']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        projectionFilter = vtkvmtk.vtkvmtkPolyDataCenterlineProjection()
        projectionFilter.SetInputData(self.Surface)
        projectionFilter.SetCenterlines(self.Centerlines)
        projectionFilter.SetUseRadiusInformation(self.UseRadiusInformation)
        projectionFilter.SetCenterlineRadiusArrayName(self.RadiusArrayName)
        projectionFilter.Update()

        self.Surface = projectionFilter.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
