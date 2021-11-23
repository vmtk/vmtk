#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkboundaryreferencesystems.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.7 $

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


class vmtkBoundaryReferenceSystems(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.ReferenceSystems = None

        self.BoundaryRadiusArrayName = 'BoundaryRadius'
        self.BoundaryNormalsArrayName = 'BoundaryNormals'
        self.Point1ArrayName = 'Point1'
        self.Point2ArrayName = 'Point2'

        self.SetScriptName('vmtkboundaryreferencesystems')
        self.SetScriptDoc('compute the reference systems relative to each open boundary of a model; a typical use is the specification of boundary conditions for CFD simulations; reference systems are given both as origin and normal, and as origin, a first point defining the x axis with the origin and a second point defining the xy plane with the former two')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface','vmtksurfacereader'],
            ['BoundaryRadiusArrayName','boundaryradiusarray','str',1,'','name of the array where the mean radius of each boundary has to be stored'],
            ['BoundaryNormalsArrayName','boundarynormalsarray','str',1,'','name of the array where outward pointing normals to each boundary have to be stored'],
            ['Point1ArrayName','point1array','str',1,'','name of the array where the coordinates of a point of each boundary have to be stored'],
            ['Point2ArrayName','point2array','str',1,'','name of the array where the coordinates of a second point of each boundary have to be stored']
            ])
        self.SetOutputMembers([
            ['ReferenceSystems','o','vtkPolyData',1,'','the output reference systems, given as points coinciding with the origins','vmtksurfacewriter'],
            ['BoundaryRadiusArrayName','boundaryradiusarray','str',1,'','name of the array where the mean radius of each boundary has to be stored'],
            ['BoundaryNormalsArrayName','boundarynormalsarray','str',1,'','name of the array where outward pointing normals to each boundary are stored'],
            ['Point1ArrayName','point1array','str',1,'','name of the array where the coordinates of a point of each boundary are stored'],
            ['Point2ArrayName','point2array','str',1,'','name of the array where the coordinates of a second point of each boundary are stored']
           ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        boundaryReferenceSystems = vtkvmtk.vtkvmtkBoundaryReferenceSystems()
        boundaryReferenceSystems.SetInputData(self.Surface)
        boundaryReferenceSystems.SetBoundaryRadiusArrayName(self.BoundaryRadiusArrayName)
        boundaryReferenceSystems.SetBoundaryNormalsArrayName(self.BoundaryNormalsArrayName)
        boundaryReferenceSystems.SetPoint1ArrayName(self.Point1ArrayName)
        boundaryReferenceSystems.SetPoint2ArrayName(self.Point2ArrayName)
        boundaryReferenceSystems.Update()

        self.ReferenceSystems = boundaryReferenceSystems.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
