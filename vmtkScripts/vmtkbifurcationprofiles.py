#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbifurcationprofiles.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

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


class vmtkBifurcationProfiles(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.BifurcationProfiles = None

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'
        self.BlankingArrayName = 'Blanking'

        self.BifurcationProfileGroupIdsArrayName = 'BifurcationProfileGroupIds'
        self.BifurcationProfileBifurcationGroupIdsArrayName = 'BifurcationProfileBifurcationGroupIds'
        self.BifurcationProfileOrientationArrayName = 'BifurcationProfileOrientation'

        self.SetScriptName('vmtkbifurcationprofiles')
        self.SetScriptDoc('compute bifurcation profiles, i.e. the bifurcation splitting lines. The script takes in input the surface and the relative centerlines, both already split into branches.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface, already split into branches','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines, already split into branches','vmtksurfacereader'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius is stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1,'','name of the array where centerline ids are stored'],
            ['TractIdsArrayName','tractidsarray','str',1,'','name of the array where centerline tract ids are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','name of the array where centerline blanking information about branches is stored'],
            ['BifurcationProfileGroupIdsArrayName','bifurcationprofilegroupids','str',1,'','name of the array where the group id to which each profile belongs has to be stored'],
            ['BifurcationProfileBifurcationGroupIdsArrayName','bifurcationprofilebifurcationgroupids','str',1,'','name of the array where the bifurcation group id to which each profile belongs has to be stored'],
            ['BifurcationProfileOrientationArrayName','bifurcationprofileorientation','str',1,'','name of the array containing 0 if a profile is upstream and 0 downstream its bifurcation']
            ])
        self.SetOutputMembers([
            ['BifurcationProfiles','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter'],
            ['BifurcationProfileGroupIdsArrayName','bifurcationprofilegroupids','str',1,'','name of the array where the group id to which each profile belongs are stored'],
            ['BifurcationProfileBifurcationGroupIdsArrayName','bifurcationprofilebifurcationgroupids','str',1,'','name of the array where the bifurcation group id to which each profile belongs has to be stored'],
            ['BifurcationProfileOrientationArrayName','bifurcationprofileorientation','str',1,'','name of the array containing 0 if a profile is upstream and 0 downstream its bifurcation']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        bifurcationProfiles = vtkvmtk.vtkvmtkPolyDataBifurcationProfiles()
        bifurcationProfiles.SetInputData(self.Surface)
        bifurcationProfiles.SetGroupIdsArrayName(self.GroupIdsArrayName)
        bifurcationProfiles.SetCenterlines(self.Centerlines)
        bifurcationProfiles.SetCenterlineRadiusArrayName(self.RadiusArrayName)
        bifurcationProfiles.SetCenterlineGroupIdsArrayName(self.GroupIdsArrayName)
        bifurcationProfiles.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        bifurcationProfiles.SetCenterlineTractIdsArrayName(self.TractIdsArrayName)
        bifurcationProfiles.SetBlankingArrayName(self.BlankingArrayName)
        bifurcationProfiles.SetBifurcationProfileGroupIdsArrayName(self.BifurcationProfileGroupIdsArrayName)
        bifurcationProfiles.SetBifurcationProfileBifurcationGroupIdsArrayName(self.BifurcationProfileBifurcationGroupIdsArrayName)
        bifurcationProfiles.SetBifurcationProfileOrientationArrayName(self.BifurcationProfileOrientationArrayName)
        bifurcationProfiles.Update()

        self.BifurcationProfiles = bifurcationProfiles.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
