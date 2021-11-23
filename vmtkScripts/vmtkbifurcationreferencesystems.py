#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbifurcationreferencesystems.py,v $
## Language:  Python
## Date:      $Date: 2005/09/14 09:48:31 $
## Version:   $Revision: 1.8 $

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


class vmtkBifurcationReferenceSystems(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Centerlines = None
        self.ReferenceSystems = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.BlankingArrayName = 'Blanking'
        self.GroupIdsArrayName = 'GroupIds'
        self.ReferenceSystemsNormalArrayName = 'Normal'
        self.ReferenceSystemsUpNormalArrayName = 'UpNormal'

        self.SetScriptName('vmtkbifurcationreferencesystems')
        self.SetScriptDoc('compute reference systems for each bifurcation of a tree. The script takes in input the centerlines already split into branches.')
        self.SetInputMembers([
            ['Centerlines','i','vtkPolyData',1,'','the input split centerlines','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','the name of the array where centerline radius values are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','the name of the array where centerline blanking information about branches is stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','the name of the array where centerline group ids are stored'],
            ['ReferenceSystemsNormalArrayName','normalarray','str',1,'','the name of the array where reference system plane normals have to be stored'],
            ['ReferenceSystemsUpNormalArrayName','upnormalarray','str',1,'','the name of the array where reference system upnormals have to be stored']
            ])
        self.SetOutputMembers([
            ['ReferenceSystems','o','vtkPolyData',1,'','the output reference systems, given as points coinciding with the origins','vmtksurfacewriter'],
            ['ReferenceSystemsNormalArrayName','normalarray','str',1,'','the name of the array where reference system plane normals are stored'],
            ['ReferenceSystemsUpNormalArrayName','upnormalarray','str',1,'','the name of the array where reference system upnormals are stored']
           ])

    def Execute(self):

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        bifurcationReferenceSystems = vtkvmtk.vtkvmtkCenterlineBifurcationReferenceSystems()
        bifurcationReferenceSystems.SetInputData(self.Centerlines)
        bifurcationReferenceSystems.SetRadiusArrayName(self.RadiusArrayName)
        bifurcationReferenceSystems.SetBlankingArrayName(self.BlankingArrayName)
        bifurcationReferenceSystems.SetGroupIdsArrayName(self.GroupIdsArrayName)
        bifurcationReferenceSystems.SetNormalArrayName(self.ReferenceSystemsNormalArrayName)
        bifurcationReferenceSystems.SetUpNormalArrayName(self.ReferenceSystemsUpNormalArrayName)
        ##	bifurcationReferenceSystems.SetReferenceGroupId(self.ReferenceGroupId)
        bifurcationReferenceSystems.Update()

        self.ReferenceSystems = bifurcationReferenceSystems.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
