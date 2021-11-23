#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbifurcationsections.py,v $
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


class vmtkBifurcationSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.Surface = None
        self.Centerlines = None
        self.BifurcationSections = None

        self.NumberOfDistanceSpheres = 1

        self.RadiusArrayName = 'MaximumInscribedSphereRadius'
        self.GroupIdsArrayName = 'GroupIds'
        self.CenterlineIdsArrayName = 'CenterlineIds'
        self.TractIdsArrayName = 'TractIds'
        self.BlankingArrayName = 'Blanking'

        self.OutputSectionPointGroupId = None
        self.OutputSectionPointBifurcationGroupId = None
        self.OutputSectionPoint = []
        self.OutputSectionNormal = []
        self.OutputSectionArea = 0.0

        self.BifurcationSectionGroupIdsArrayName = 'BifurcationSectionGroupIds'
        self.BifurcationSectionBifurcationGroupIdsArrayName = 'BifurcationSectionBifurcationGroupIds'
        self.BifurcationSectionPointArrayName = 'BifurcationSectionPoint'
        self.BifurcationSectionNormalArrayName = 'BifurcationSectionNormal'
        self.BifurcationSectionAreaArrayName = 'BifurcationSectionArea'
        self.BifurcationSectionMinSizeArrayName = 'BifurcationSectionMinSize'
        self.BifurcationSectionMaxSizeArrayName = 'BifurcationSectionMaxSize'
        self.BifurcationSectionShapeArrayName = 'BifurcationSectionShape'
        self.BifurcationSectionClosedArrayName = 'BifurcationSectionClosed'
        self.BifurcationSectionOrientationArrayName = 'BifurcationSectionOrientation'
        self.BifurcationSectionDistanceSpheresArrayName = 'BifurcationSectionDistanceSpheres'

        self.SetScriptName('vmtkbifurcationsections')
        self.SetScriptDoc('compute branch sections located a fixed number of spheres away from each bifurcation. The script takes in input the surface and the relative centerlines, both already split into branches.')
        self.SetInputMembers([
            ['Surface','i','vtkPolyData',1,'','the input surface, already split into branches','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'','the input centerlines, already split into branches','vmtksurfacereader'],
            ['NumberOfDistanceSpheres','distancespheres','int',1,'','distance from the bifurcation at which the sections have to be taken; the distance is expressed in number of inscribed spheres, where each sphere touches the center of the previous one'],
            ['RadiusArrayName','radiusarray','str',1,'','name of the array where centerline radius is stored'],
            ['GroupIdsArrayName','groupidsarray','str',1,'','name of the array where centerline group ids are stored'],
            ['CenterlineIdsArrayName','centerlineidsarray','str',1,'','name of the array where centerline ids are stored'],
            ['TractIdsArrayName','tractidsarray','str',1,'','name of the array where centerline tract ids are stored'],
            ['BlankingArrayName','blankingarray','str',1,'','name of the array where centerline blanking information about branches is stored'],
            ['BifurcationSectionGroupIdsArrayName','bifurcationsectiongroupids','str',1,'','name of the array where the group id to which each section belongs has to be stored'],
            ['BifurcationSectionBifurcationGroupIdsArrayName','bifurcationsectionbifurcationgroupids','str',1,'','name of the array where the bifurcation group id to which each section belongs has to be stored'],
            ['BifurcationSectionPointArrayName','bifurcationsectionpoint','str',1,'','name of the array where the point at which bifurcation sections are defined have to be stored'],
            ['BifurcationSectionNormalArrayName','bifurcationsectionnormal','str',1,'','name of the array where the bifurcation section normals have to be stored'],
            ['BifurcationSectionAreaArrayName','bifurcationsectionarea','str',1,'','name of the array where the area of bifurcation sections have to be stored'],
            ['BifurcationSectionMinSizeArrayName','bifurcationsectionminsize','str',1,'','name of the array where the minimum diameter of each section has to be stored'],
            ['BifurcationSectionMaxSizeArrayName','bifurcationsectionmaxsize','str',1,'','name of the array where the maximum diameter of each bifurcation sections has to be stored'],
            ['BifurcationSectionShapeArrayName','bifurcationsectionshape','str',1,'','name of the array where the shape index, i.e. the ratio between minimum and maximum diameter, of each bifurcation section has to be stored'],
            ['BifurcationSectionClosedArrayName','bifurcationsectionclosed','str',1,'','name of the array containing 1 if a section is closed and 0 otherwise'],
            ['BifurcationSectionOrientationArrayName','bifurcationsectionorientation','str',1,'','name of the array containing 0 if a section is upstream and 0 downstream its bifurcation'],
            ['BifurcationSectionDistanceSpheresArrayName','bifurcationsectiondistancespheres','str',1,'','name of the array containing the number of spheres away from the bifurcation the section is located at'],
            ['OutputSectionPointGroupId','sectionpointgroupid','int',1,'(0,)'],
            ['OutputSectionPointBifurcationGroupId','sectionpointbifurcationgroupid','int',1,'(0,)']
            ])
        self.SetOutputMembers([
            ['BifurcationSections','o','vtkPolyData',1,'','the output sections','vmtksurfacewriter'],
            ['BifurcationSectionGroupIdsArrayName','bifurcationsectiongroupids','str',1,'','name of the array where the group id to which each section belongs are stored'],
            ['BifurcationSectionBifurcationGroupIdsArrayName','bifurcationsectionbifurcationgroupids','str',1,'','name of the array where the bifurcation group id to which each section belongs has to be stored'],
            ['BifurcationSectionPointArrayName','bifurcationsectionpoint','str',1,'','name of the array where the point at which bifurcation sections are defined are stored'],
            ['BifurcationSectionNormalArrayName','bifurcationsectionnormal','str',1,'','name of the array where bifurcation section normals are stored'],
            ['BifurcationSectionAreaArrayName','bifurcationsectionarea','str',1,'','name of the array where the area of bifurcation sections are stored'],
            ['BifurcationSectionMinSizeArrayName','bifurcationsectionminsize','str',1,'','name of the array where the minimum diameter of each section are stored'],
            ['BifurcationSectionMaxSizeArrayName','bifurcationsectionmaxsize','str',1,'','name of the array where the minimum diameter of each bifurcation sections has to be stored'],
            ['BifurcationSectionShapeArrayName','bifurcationsectionshape','str',1,'','name of the array where the shape index, i.e. the ratio between minimum and maximum diameter, of each bifurcation section are stored'],
            ['BifurcationSectionClosedArrayName','bifurcationsectionclosed','str',1,'','name of the array containing 1 if a section is closed and 0 otherwise'],
            ['BifurcationSectionOrientationArrayName','bifurcationsectionorientation','str',1,'','name of the array containing 0 if a section is upstream and 0 downstream its bifurcation'],
            ['BifurcationSectionDistanceSpheresArrayName','bifurcationsectiondistancespheres','str',1,'','name of the array containing the number of spheres away from the bifurcation the section is located at'],
            ['OutputSectionPoint','sectionpoint','float',3],
            ['OutputSectionNormal','sectionnormal','float',3],
            ['OutputSectionArea','sectionarea','float',1]
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        bifurcationSections = vtkvmtk.vtkvmtkPolyDataBifurcationSections()
        bifurcationSections.SetInputData(self.Surface)
        bifurcationSections.SetGroupIdsArrayName(self.GroupIdsArrayName)
        bifurcationSections.SetCenterlines(self.Centerlines)
        bifurcationSections.SetNumberOfDistanceSpheres(self.NumberOfDistanceSpheres)
        bifurcationSections.SetCenterlineRadiusArrayName(self.RadiusArrayName)
        bifurcationSections.SetCenterlineGroupIdsArrayName(self.GroupIdsArrayName)
        bifurcationSections.SetCenterlineIdsArrayName(self.CenterlineIdsArrayName)
        bifurcationSections.SetCenterlineTractIdsArrayName(self.TractIdsArrayName)
        bifurcationSections.SetBlankingArrayName(self.BlankingArrayName)
        bifurcationSections.SetBifurcationSectionGroupIdsArrayName(self.BifurcationSectionGroupIdsArrayName)
        bifurcationSections.SetBifurcationSectionBifurcationGroupIdsArrayName(self.BifurcationSectionBifurcationGroupIdsArrayName)
        bifurcationSections.SetBifurcationSectionPointArrayName(self.BifurcationSectionPointArrayName)
        bifurcationSections.SetBifurcationSectionNormalArrayName(self.BifurcationSectionNormalArrayName)
        bifurcationSections.SetBifurcationSectionAreaArrayName(self.BifurcationSectionAreaArrayName)
        bifurcationSections.SetBifurcationSectionMinSizeArrayName(self.BifurcationSectionMinSizeArrayName)
        bifurcationSections.SetBifurcationSectionMaxSizeArrayName(self.BifurcationSectionMaxSizeArrayName)
        bifurcationSections.SetBifurcationSectionShapeArrayName(self.BifurcationSectionShapeArrayName)
        bifurcationSections.SetBifurcationSectionClosedArrayName(self.BifurcationSectionClosedArrayName)
        bifurcationSections.SetBifurcationSectionOrientationArrayName(self.BifurcationSectionOrientationArrayName)
        bifurcationSections.SetBifurcationSectionDistanceSpheresArrayName(self.BifurcationSectionDistanceSpheresArrayName)
        bifurcationSections.Update()

        self.BifurcationSections = bifurcationSections.GetOutput()

        if self.OutputSectionPointGroupId != None and self.OutputSectionPointBifurcationGroupId != None:
            groupIds = self.BifurcationSections.GetCellData().GetArray(self.BifurcationSectionGroupIdsArrayName)
            bifurcationGroupIds = self.BifurcationSections.GetCellData().GetArray(self.BifurcationSectionBifurcationGroupIdsArrayName)
            for i in range(self.BifurcationSections.GetNumberOfCells()):
                if int(groupIds.GetTuple1(i)) == self.OutputSectionPointGroupId and int(bifurcationGroupIds.GetTuple1(i)) == self.OutputSectionPointBifurcationGroupId:
                    self.OutputSectionPoint = self.BifurcationSections.GetCellData().GetArray(self.BifurcationSectionPointArrayName).GetTuple3(i)
                    self.OutputSectionNormal = self.BifurcationSections.GetCellData().GetArray(self.BifurcationSectionNormalArrayName).GetTuple3(i)
                    self.OutputSectionArea = self.BifurcationSections.GetCellData().GetArray(self.BifurcationSectionAreaArrayName).GetTuple1(i)
                    self.InputInfo('SectionPoint: '+str(self.OutputSectionPoint))
                    self.InputInfo('SectionNormal: '+str(self.OutputSectionNormal))
                    self.InputInfo('SectionArea: '+str(self.OutputSectionArea))


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
