#!/usr/bin/env python

## Program:   VMTK
## Module:    $RCSfile: vmtkbifurcationsections.py,v $
## Language:  Python
## Date:      $Date: 2006/10/17 15:16:16 $
## Version:   $Revision: 1.1 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import vtk
import vtkvmtk
import sys

import pypes

vmtkbifurcationsections = 'vmtkBifurcationSections'

class vmtkBifurcationSections(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)
       
        self.Surface = None
        self.Centerlines = None
        self.BifurcationSections = None

        self.NumberOfDistanceSpheres = 1

        self.RadiusArrayName = ''
        self.GroupIdsArrayName = ''
        self.CenterlineIdsArrayName = ''
        self.TractIdsArrayName = ''
        self.BlankingArrayName = ''

        self.BifurcationSectionGroupIdsArrayName = 'BifurcationSectionGroupIds'
        self.BifurcationSectionBifurcationGroupIdsArrayName = 'BifurcationSectionBifurcationGroupIds'
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
            ['Surface','i','vtkPolyData',1,'the input surface, already split into branches','vmtksurfacereader'],
            ['Centerlines','centerlines','vtkPolyData',1,'the input centerlines, already split into branches','vmtksurfacereader'],
      	    ['NumberOfDistanceSpheres','distancespheres','int',1,'distance from the bifurcation at which the sections have to be taken; the distance is expressed in number of inscribed spheres, where each sphere touches the center of the previous one'],
      	    ['RadiusArrayName','radiusarray','str',1,'name of the array where centerline radius is stored'],
      	    ['GroupIdsArrayName','groupidsarray','str',1,'name of the array where centerline group ids are stored'],
      	    ['CenterlineIdsArrayName','centerlineidsarray','str',1,'name of the array where centerline ids are stored'],
      	    ['TractIdsArrayName','tractidsarray','str',1,'name of the array where centerline tract ids are stored'],
      	    ['BlankingArrayName','blankingarray','str',1,'name of the array where centerline blanking information about branches is stored'],
      	    ['BifurcationSectionGroupIdsArrayName','bifurcationsectiongroupids','str',1,'name of the array where the group id to which each section belongs has to be stored'],
      	    ['BifurcationSectionBifurcationGroupIdsArrayName','bifurcationsectionbifurcationgroupids','str',1,'name of the array where the bifurcation group id to which each section belongs has to be stored'],
      	    ['BifurcationSectionAreaArrayName','bifurcationsectionarea','str',1,'name of the array where the area of bifurcation sections have to be stored'],
      	    ['BifurcationSectionMinSizeArrayName','bifurcationsectionminsize','str',1,'name of the array where the minvmtkm diameter of each section has to be stored'],
      	    ['BifurcationSectionMaxSizeArrayName','bifurcationsectionmaxsize','str',1,'name of the array where the maxvmtkm diameter of each bifurcation sections has to be stored'],
      	    ['BifurcationSectionShapeArrayName','bifurcationsectionshape','str',1,'name of the array where the shape index, i.e. the ratio between minvmtkm and maxvmtkm diameter, of each bifurcation section has to be stored'],
      	    ['BifurcationSectionClosedArrayName','bifurcationsectionclosed','str',1,'name of the array containing 1 if a section is closed and 0 otherwise'],
      	    ['BifurcationSectionOrientationArrayName','bifurcationsectionorientation','str',1,'name of the array containing 0 if a section is upstream and 0 downstream its bifurcation'],
      	    ['BifurcationSectionDistanceSpheresArrayName','bifurcationsectiondistancespheres','str',1,'name of the array containing the number of spheres away from the bifurcation the section is located at']
            ])
        self.SetOutputMembers([
            ['BifurcationSections','o','vtkPolyData',1,'the output sections','vmtksurfacewriter'],
      	    ['BifurcationSectionGroupIdsArrayName','bifurcationsectiongroupids','str',1,'name of the array where the group id to which each section belongs are stored'],
      	    ['BifurcationSectionBifurcationGroupIdsArrayName','bifurcationsectionbifurcationgroupids','str',1,'name of the array where the bifurcation group id to which each section belongs has to be stored'],
      	    ['BifurcationSectionAreaArrayName','bifurcationsectionarea','str',1,'name of the array where the area of bifurcation sections are stored'],
      	    ['BifurcationSectionMinSizeArrayName','bifurcationsectionminsize','str',1,'name of the array where the minvmtkm diameter of each section are stored'],
      	    ['BifurcationSectionMaxSizeArrayName','bifurcationsectionmaxsize','str',1,'name of the array where the minvmtkm diameter of each bifurcation sections has to be stored'],
      	    ['BifurcationSectionShapeArrayName','bifurcationsectionshape','str',1,'name of the array where the shape index, i.e. the ratio between minvmtkm and maxvmtkm diameter, of each bifurcation section are stored'],
      	    ['BifurcationSectionClosedArrayName','bifurcationsectionclosed','str',1,'name of the array containing 1 if a section is closed and 0 otherwise'],
      	    ['BifurcationSectionOrientationArrayName','bifurcationsectionorientation','str',1,'name of the array containing 0 if a section is upstream and 0 downstream its bifurcation'],
      	    ['BifurcationSectionDistanceSpheresArrayName','bifurcationsectiondistancespheres','str',1,'name of the array containing the number of spheres away from the bifurcation the section is located at']
            ])

    def Execute(self):

        if self.Surface == None:
            self.PrintError('Error: No input surface.')

        if self.Centerlines == None:
            self.PrintError('Error: No input centerlines.')

        bifurcationSections = vtkvmtk.vtkvmtkPolyDataBifurcationSections()
        bifurcationSections.SetInput(self.Surface)
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
        bifurcationSections.SetBifurcationSectionAreaArrayName(self.BifurcationSectionAreaArrayName)
        bifurcationSections.SetBifurcationSectionMinSizeArrayName(self.BifurcationSectionMinSizeArrayName)
        bifurcationSections.SetBifurcationSectionMaxSizeArrayName(self.BifurcationSectionMaxSizeArrayName)
        bifurcationSections.SetBifurcationSectionShapeArrayName(self.BifurcationSectionShapeArrayName)
        bifurcationSections.SetBifurcationSectionClosedArrayName(self.BifurcationSectionClosedArrayName)
        bifurcationSections.SetBifurcationSectionOrientationArrayName(self.BifurcationSectionOrientationArrayName)
        bifurcationSections.SetBifurcationSectionDistanceSpheresArrayName(self.BifurcationSectionDistanceSpheresArrayName)
        bifurcationSections.Update()

        self.BifurcationSections = bifurcationSections.GetOutput()


if __name__=='__main__':

    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
